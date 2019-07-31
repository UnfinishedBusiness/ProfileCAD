/*! ProfileGraphics - Travis Gillin */

;(function (global) {

// Compiler directive for UglifyJS.  See MotionPlanner.const.js for more info.
if (typeof DEBUG === 'undefined') {
  DEBUG = true;
}


// MotionPlanner-GLOBAL CONSTANTS
//
// These constants are exposed to all MotionPlanner modules.


// GLOBAL is a reference to the global Object.
var Fn = Function, GLOBAL = new Fn('return this')();


// MotionPlanner-GLOBAL METHODS
//
// The methods here are exposed to all MotionPlanner modules.  Because all of the
// source files are wrapped within a closure at build time, they are not
// exposed globally in the distributable binaries.


/**
 * A no-op function.  Useful for passing around as a default callback.
 */
function noop () { }


/**
 * Init wrapper for the core module.
 * @param {Object} The Object that the MotionPlanner gets attached to in
 * MotionPlanner.init.js.  If the MotionPlanner was not loaded with an AMD loader such as
 * require.js, this is the global Object.
 */
function initMotionPlannerCore (context) {


  // It is recommended to use strict mode to help make mistakes easier to find.
  'use strict';


  // PRIVATE MODULE CONSTANTS
  //


  // An example of a CONSTANT variable;
  var CORE_CONSTANT = true;


  // PRIVATE MODULE METHODS
  //
  // These do not get attached to a prototype.  They are private utility
  // functions.


  /**
   *  An example of a private method.  Feel free to remove this.
   *  @param {number} aNumber This is a parameter description.
   *  @returns {number} This is a return value description.
   */
  function corePrivateMethod (aNumber) {
    return aNumber;
  }


  /**
   * This is the constructor for the MotionPlanner Object.  Please rename it to
   * whatever your MotionPlanner's name is.  Note that the constructor is also being
   * attached to the context that the MotionPlanner was loaded in.
   * @param {Object} opt_config Contains any properties that should be used to
   * configure this instance of the MotionPlanner.
   * @constructor
   */
  var MotionPlanner = context.MotionPlanner = function(opt_config) {

    opt_config = opt_config || {};

    // INSTANCE PROPERTY SETUP
    //
    // Your MotionPlanner likely has some instance-specific properties.  The value of
    // these properties can depend on any number of things, such as properties
    // passed in via opt_config or global state.  Whatever the case, the values
    // should be set in this constructor.

    // Instance variables that have a leading underscore mean that they should
    // not be modified outside of the MotionPlanner.  They can be freely modified
    // internally, however.  If an instance variable will likely be accessed
    // outside of the MotionPlanner, consider making a public getter function for it.
    //this._readOnlyVar = 'read only';
    this._Feed_Sample_Timestamp = 0;
    this._Feedrate_Timestamp = 0;
    this._Feedrate_delay = 250;
    this._Step_Scale = {x: 50, y: 50}; //This is the ammount of steps required to go one unit
    this._Feed_Jerk = {x: 0.08, y: 0.08}; //The minimum feedrate that each axis needs to deccelate to change directions
    this._Feed_Accel = {x: 0.8, y: 0.5}; //The minimum feedrate that each axis needs to deccelate to change directions
    this.Motion = { dx: 0, dy: 0, sx: 0, sy: 0, err: 0, x_stg: 0, y_stg: 0, run: false};
    this.Callbacks = {};
    this.Callbacks.Step_X = function(dir){};
    this.Callbacks.Step_Y = function(dir){};
    this.CurrentPosition = {x: 0, y: 0};
    this.TargetPosition = {x: 0, y: 0};
    this.CurrentVelocity = {x: 0, y: 0};
    this.percentage_into_move = 0;
    this.CurrentMove = {};

    this.Stack = []; //Holds the pending moves. As moves are completed, they are shifted off the top of the stack

    return this;
  };
  /*
    Push a new target position to the stack with f as the target feedrate in inches/minute
  */
  MotionPlanner.prototype.push_target = function(position)
  {
    this.Stack.push({x: Math.trunc(position.x * this._Step_Scale.x), y: Math.trunc(position.y * this._Step_Scale.y), f: position.f / 60});
  }
  MotionPlanner.prototype.get_current_position = function()
  {
    return {x: this.CurrentPosition.x / this._Step_Scale.x, y: this.CurrentPosition.y / this._Step_Scale.y};
  }
  MotionPlanner.prototype.get_current_velocity = function()
  {
    var current_vel = Math.sqrt( Math.pow((this.CurrentVelocity.x), 2) + Math.pow((this.CurrentVelocity.y), 2));
    console.log("Cartesion Vel (inches/min): " + (current_vel * 60));
    return current_vel;
  }
  /*
    We need to calculate and set the this._Feedrate_delay to reflect our desired feedrate in inches/sec
    //This is called by the motion interupt, not meant to be user callable. Set target feedrates when pushing moves to the stack
  */
  MotionPlanner.prototype.set_feedrate = function(feed)
  {
    if (this.Motion.dx == 0 && this.Motion.dy == 0) return;
    var x_dist_inches = this.Motion.dx / this._Step_Scale.x;
    var y_dist_inches = this.Motion.dy / this._Step_Scale.y;
    var cartesion_distance = Math.sqrt(Math.pow((x_dist_inches), 2) + Math.pow(y_dist_inches, 2));
    var move_duration_sec = cartesion_distance / feed;
    var number_of_cycles = this.Motion.dx;
    if (this.Motion.dy > number_of_cycles) number_of_cycles = this.Motion.dy;
    //console.log("Number of cycles: " + number_of_cycles);
    this._Feedrate_delay = (move_duration_sec * 1000) / number_of_cycles;
    this.CurrentVelocity.x = x_dist_inches / move_duration_sec;
    this.CurrentVelocity.y = y_dist_inches / move_duration_sec;
  }
  /*
  Calculate all the ramp maps for the moves that are currently on the stack, then execute the motion
  */
  MotionPlanner.prototype.plan_moves_on_stack_and_run = function()
  {
    //First pass - Iterate all the moves on the stack and calculate the ramp_maps in exact stop mode mode
    var current_position = this.copy_obj(this.CurrentPosition); //Current position is are physical position when everything is planned
    for (var x = 0; x < this.Stack.length; x++)
    {
      var move = this.Stack[x];
      move.ramp_map = this.motion_calculate_ramp_map(Math.abs(current_position.x - move.x) / this._Step_Scale.x, Math.abs(current_position.y - move.y) / this._Step_Scale.y, move.f);
      current_position = move; //Current position is now the position of the last move
    }
    //Second pass - Iterate all the moves on the stack and deterime if the vector angle change is enough to need to decelerate to _Feed_Jerk
    var last_vector_polar_angle = 0;
    for (var x = 1; x < this.Stack.length; x++)
    {
      var dominent_axis_jerk = this._Feed_Jerk.x; //Assume X then update to Y if its the dominent axis
      var x_dist_inches = Math.abs(this.Stack[x-1].x - this.Stack[x].x);
      var y_dist_inches = Math.abs(this.Stack[x-1].y - this.Stack[x].y);
      if (y_dist_inches > x_dist_inches) dominent_axis_jerk = this._Feed_Jerk.y;
      var vector_polor_angle = this.get_line_angle({x: this.Stack[x-1].x,y: this.Stack[x-1].y}, {x: this.Stack[x].x,y: this.Stack[x].y});
      var angle_of_change = Math.abs(vector_polor_angle - last_vector_polar_angle);
      if (angle_of_change > 180) angle_of_change = 180; //Clamp angle of change to 180 degress because our map function for calculating the exit velocity will only expect up to 180
      //console.log("Vector angle of change: " + angle_of_change);
      var exit_velocity = this.map(angle_of_change, 0, 180, this.Stack[x-1].f, dominent_axis_jerk);
      //console.log("Suggested Exit Velocity: " + exit_velocity);
      //Iterate the ramp_map of this.Stack[x-1] and find index where peak velocity was. Then find index where our calculated exit velocity was
      //Move this block of ramps to the end of the stack so that exit velocity is our new exit velocity on the ramp stack. Backfill old decel items with peak_velocity
      //Now do the opposite of the above on this.Stack[x] so that the entry volocity is a mirror of the previous exit ramp
      this.motion_recalculate_ramp_map_for_exit(this.Stack[x-1].ramp_map, exit_velocity);
      //Exit velocity of this.Stack[x-1] is the entry velocity of this.Stack[x]
      this.motion_recalculate_ramp_map_for_entry(this.Stack[x].ramp_map, exit_velocity);
      last_vector_polar_angle = vector_polor_angle;
    }
  }
  /*
  Update ramp_map for entry velocity
  target_velocity is inches/sec
  */
  MotionPlanner.prototype.motion_recalculate_ramp_map_for_entry = function(ramp, entry_velocity)
  {
    var biggest_feed = 0;
    var biggest_feed_index = 0;
    for (var y = 0; y < Math.trunc(ramp.length/2); y++) //Only look at the first half of the ramp map where we are accelerating
    {
      if (ramp[y] > biggest_feed) //Save the index where the ramp starts to deccel
      {
        biggest_feed = ramp[y];
        biggest_feed_index = y;
      }
    }
    var entry_velocity_index = 0;
    for (var y = 0; y < Math.trunc(ramp.length/2); y++) //Iterate forwards until we find a value bigger than entry_volocity and break
    {
      if (ramp[y] > entry_velocity)
      {
        entry_velocity_index = y-1;
        break;
      }
    }
    var new_decel_ramp = [];
    for (var y = 0; y < Math.trunc(ramp.length/2); y++)
    {
      if (y > entry_velocity_index)
      {
        new_decel_ramp.push(ramp[y]); //Copy this part of the decel ramp to a stack
      }
      ramp[y] = biggest_feed; //Override the rest of the stack with the biggest feedrate
    }
    //copy the new_decel_ramp to the beggining of ramp
    var new_ramp_position = 0;
    for (var y = 0; y < new_decel_ramp.length; y++)
    {
      ramp[y] = new_decel_ramp[new_ramp_position];
      new_ramp_position++;
    }
  }
  /*
  Update ramp_map for exit velocity
  target_velocity is inches/sec
  */
  MotionPlanner.prototype.motion_recalculate_ramp_map_for_exit = function(ramp, exit_velocity)
  {
    var biggest_feed = 0;
    var biggest_feed_index = 0;
    for (var y = Math.trunc(ramp.length/2); y < ramp.length; y++) //Only look at last half of map where decel could be happening
    {
      if (ramp[y] > biggest_feed) //Save the index where the ramp starts to deccel
      {
        biggest_feed = ramp[y];
        biggest_feed_index = y;
      }
    }
    var exit_velocity_index = 0;
    for (var y = ramp.length; y > Math.trunc(ramp.length/2); y--) //Iterate backwords until we find a value bigger than exit_volocity and break
    {
      if (ramp[y] > exit_velocity)
      {
        exit_velocity_index = y+1;
        break;
      }
    }
    var new_decel_ramp = [];
    for (var y = biggest_feed_index; y < ramp.length; y++)
    {
      if (y < exit_velocity_index)
      {
        new_decel_ramp.push(ramp[y]); //Copy this part of the decel ramp to a stack
      }
      ramp[y] = biggest_feed; //Override the rest of the stack with the biggest feedrate
    }
    //copy the new_decel_ramp to the end of the ramp
    var new_ramp_position = 0;
    for (var y = (ramp.length - new_decel_ramp.length); y < ramp.length; y++)
    {
      ramp[y] = new_decel_ramp[new_ramp_position];
      new_ramp_position++;
    }

  }
  /*
  Update CurrentMove.ramp_map - add a deccel to stop and an accel from stop to target velocity
  just ahead of current position in ramp_map
  target_velocity is inches/sec
  */
  MotionPlanner.prototype.insert_feedhold = function()
  {
    var ramp_map = this.motion_calculate_ramp_map(this.Move.dx / this._Step_Scale.x, this.Move.dy / this._Step_Scale.y, this.CurrentMove.f);

  }
  /*
  Calculate a velocity map from zero to target volocity then back to zero in order of percentage complete through move.
  Does not take next moves into consideration, motion_update_planner with overite part of these values later...

  This would be exact stop mode

  target_velocity is inches/sec
  */
  MotionPlanner.prototype.motion_calculate_ramp_map = function(x_dist_inches, y_dist_inches, target_velocity)
  {
    if (x_dist_inches == 0 && y_dist_inches == 0) return;
    var cartesion_distance = Math.sqrt(Math.pow((x_dist_inches), 2) + Math.pow(y_dist_inches, 2));
    //console.log("cartesion_distance: " + cartesion_distance);
    var dominent_axis_accel = this._Feed_Accel.x; //Assume X then update to Y if its the dominent axis
    if (y_dist_inches > x_dist_inches) dominent_axis_accel = this._Feed_Accel.y;
    var dominent_axis_jerk = this._Feed_Jerk.x; //Assume X then update to Y if its the dominent axis
    if (y_dist_inches > x_dist_inches) dominent_axis_jerk = this._Feed_Jerk.y;
    var map = [];
    for (var percentage = 0; percentage < 100; percentage+=0.1) //Plot a feedrate value for each whole percentage
    {
      var distance_in = cartesion_distance * (percentage / 100);
      var distance_left = cartesion_distance - distance_in;
      //console.log("Percentage: " + percentage + " Distance in: " + distance_in + " Distance Left: " + distance_left);
      if (percentage < 50) //If we are less that half way through the move, only worry about acceleration
      {
        var accel_time = Math.sqrt((0.5 * dominent_axis_accel) * distance_in) * (1/(0.5 * dominent_axis_accel));
        //console.log("Accel time: " + accel_time);
        var velocity_at_this_percentage = dominent_axis_accel * accel_time;
        if (velocity_at_this_percentage > target_velocity) velocity_at_this_percentage = target_velocity; //cap velocity at the target feedrate;
        if (velocity_at_this_percentage < dominent_axis_jerk) velocity_at_this_percentage = dominent_axis_jerk;
        //console.log("Velocity at this percentage through move: " + velocity_at_this_percentage);
        map.push(velocity_at_this_percentage);
      }
      else //We need to start thinking about how much distance is required to decelerate from our current velocity to a stop
      {
        var accel_time = Math.sqrt((0.5 * dominent_axis_accel) * distance_left) * (1/(0.5 * dominent_axis_accel));
        var velocity_at_this_percentage = dominent_axis_accel * accel_time;
        if (velocity_at_this_percentage > target_velocity) velocity_at_this_percentage = target_velocity; //cap velocity at the target feedrate;
        if (velocity_at_this_percentage < dominent_axis_jerk) velocity_at_this_percentage = dominent_axis_jerk; //Make sure feedrate doesn't go below the dominent_axis_jerk
        map.push(velocity_at_this_percentage);
      }
    }
    //console.log(map);
    return map;
  }
  MotionPlanner.prototype.motion_set_target = function(x, y)
  {
    this.TargetPosition.x = x;
    this.TargetPosition.y = y;
    this.Motion.dx = Math.abs(this.TargetPosition.x - this.CurrentPosition.x), this.Motion.sx = this.CurrentPosition.x < this.TargetPosition.x ? 1 : -1;
    this.Motion.dy = Math.abs(this.TargetPosition.y - this.CurrentPosition.y), this.Motion.sy = this.CurrentPosition.y < this.TargetPosition.y ? 1 : -1;
    this.Motion.err = (this.Motion.dx>this.Motion.dy ? this.Motion.dx : -this.Motion.dy)/2;
    this.Motion.x_stg = Math.abs(this.copy_obj(this.Motion.dx));
    this.Motion.y_stg = Math.abs(this.copy_obj(this.Motion.dy));
  }
  MotionPlanner.prototype.motion_tick = function()
  {
    if (this.Motion.run == true)
    {
      if (Date.now() > this._Feed_Sample_Timestamp + 25) //Sample every x milliseconds, calculate the percentage of move complete and set the feedrate to the pre-calculated feedrate map
      {
        var dominent_axis_stg = this.Motion.x_stg;
        if (this.Motion.dy > this.Motion.dx)
        {
          dominent_axis_stg = this.Motion.y_stg;
        }
        if (dominent_axis_stg > 0)
        {
          var dominent_axis_steps = this.Motion.dx;
          var dominent_stg = this.Motion.x_stg;
          if (this.Motion.dy > this.Motion.dx)
          {
            dominent_axis_steps = this.Motion.dy;
            dominent_stg = this.Motion.y_stg;
          }
          this.percentage_into_move = Math.trunc(this.map(dominent_stg / dominent_axis_steps, 1, 0, 0, 1001));
          //console.log("Percentage through move: " + percentage_into_move);
          this.set_feedrate(this.Motion.ramp_map[this.percentage_into_move]);
          this.get_current_velocity();
        }
        this._Feed_Sample_Timestamp = Date.now();
      }
      if (Date.now() > this._Feedrate_Timestamp + this._Feedrate_delay)
      {
        var dominent_axis_stg = this.Motion.x_stg;
        if (this.Motion.dy > this.Motion.dx)
        {
          dominent_axis_stg = this.Motion.y_stg;
        }
        if (dominent_axis_stg > 0)
        {
          if (this.Motion.err > -this.Motion.dx) { this.Motion.err -= this.Motion.dy; this.CurrentPosition.x += this.Motion.sx; this.Motion.x_stg--; this.Callbacks.Step_X(); }
          if (this.Motion.err < this.Motion.dy) { this.Motion.err += this.Motion.dx; this.CurrentPosition.y += this.Motion.sy; this.Motion.y_stg--; this.Callbacks.Step_Y(); }
          //console.log("X STG: " + this.Motion.x_stg, "Y STG: " + this.Motion.y_stg + " Current X: " + this.CurrentPosition.x + " Current Y: " + this.CurrentPosition.y);
        }
        else
        {
          if (this.Stack.length > 0) //There are pending moves on the stack!
          {
            this.CurrentMove = this.Stack.shift();
            this.motion_set_target(this.CurrentMove.x, this.CurrentMove.y);
            this.Motion.ramp_map = this.CurrentMove.ramp_map;
            //this.motion_calculate_ramp_map(next_move); //This can't be done here, needs to be done by planner ahead of time
            //this.set_feedrate(next_move.f); //Set feedrate to first element of ramp feedrate
          }
          else //There are no pending moves on the stack!
          {
            this.CurrentVelocity.x = 0;
            this.CurrentVelocity.y = 0;
            this.Motion.run = false;
          }
          //console.log("End of move");
          //shift move from stack and traverse to next position
        }
        this._Feedrate_Timestamp = Date.now();
      }
    }
  }
  // These methods define the public API.
    /**
    * Return a copy of an object that arn't linked
    * @return {object}
    */
    MotionPlanner.prototype.copy_obj = function(obj) {
      return JSON.parse(JSON.stringify(obj));
    };
    /**
    * Map a value betweeen ranges
    * @return {var}
    */
    MotionPlanner.prototype.map = function(x, in_min, in_max, out_min, out_max)
    {
      return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }

  	MotionPlanner.prototype.polarToCartesian = function(centerX, centerY, radius, angleInDegrees) {
  		var angleInRadians = (angleInDegrees) * Math.PI / 180.0;

  		return {
  			x: centerX + (radius * Math.cos(angleInRadians)),
  			y: centerY + (radius * Math.sin(angleInRadians))
  		};
  	}
  	MotionPlanner.prototype.to_radians = function(degrees)
  	{
    	return degrees * Math.PI / 180;
  	}
  	MotionPlanner.prototype.to_degrees = function(radians)
  	{
    	return radians * 180 / Math.PI;
  	}
  	MotionPlanner.prototype.midpoint = function(p1, p2)
  	{
    	return {x: (p1.x+p2.x)/2, y:  (p1.y+p2.y)/2};
  	}
  	MotionPlanner.prototype.distance = function(p1, p2)
  	{
  		return Math.sqrt( Math.pow((p1.x-p2.x), 2) + Math.pow((p1.y-p2.y), 2));
  	}
  	MotionPlanner.prototype.get_line_angle = function(p1, p2)
  	{
  		var angle = this.to_degrees(Math.atan2(p1.y - p2.y, p1.x - p2.x));
  		angle += 180;
  		if (angle >= 360) angle -= 360;
  		return angle;
  	}
  	MotionPlanner.prototype.rotate_point = function(p, origin, angle)
  	{
  		var radians = (Math.PI / 180) * angle,
      cos = Math.cos(radians),
      sin = Math.sin(radians),
      nx = (cos * (p.x - origin.x)) + (sin * (p.y - origin.y)) + origin.x,
      ny = (cos * (p.y - origin.y)) - (sin * (p.x - origin.x)) + origin.y;
  		return { x: nx, y: ny };
  	}
  	MotionPlanner.prototype.get_line_at_angle = function(origin, angle, distance)
  	{
  		var new_endpoint = { x: origin.x + (Math.abs(distance) * Math.cos(this.to_radians(angle))), y: origin.y + (Math.abs(distance) * Math.sin(this.to_radians(angle))) };
    	return { type: 'line', origin: [origin.x, origin.y], end: [new_endpoint.x, new_endpoint.y] };
  	}
  	MotionPlanner.prototype.mirror_point_about_line = function(p, p0, p1)
  	{
       var dx, dy, a, b, x, y;
       dx = p1.x - p0.x;
       dy = p1.y - p0.y;
       a = (dx * dx - dy * dy) / (dx * dx + dy * dy);
       b = 2 * dx * dy / (dx * dx + dy * dy);
       x = a * (p.x - p0.x) + b * (p.y - p0.y) + p0.x;
       y = b * (p.x - p0.x) - a * (p.y - p0.y) + p0.y;
       return { x:x, y:y };
    }
  	MotionPlanner.prototype.CircleCenterFromThreePoints = function(A,B,C)
  	{
      var yDelta_a = B.y - A.y;
      var xDelta_a = B.x - A.x;
      var yDelta_b = C.y - B.y;
      var xDelta_b = C.x - B.x;

      var center = [];

      var aSlope = yDelta_a / xDelta_a;
      var bSlope = yDelta_b / xDelta_b;

      center.x = (aSlope*bSlope*(A.y - C.y) + bSlope*(A.x + B.x) - aSlope*(B.x+C.x) )/(2* (bSlope-aSlope) );
      center.y = -1*(center.x - (A.x+B.x)/2)/aSlope +  (A.y+B.y)/2;
      return center;


  	}

  /**
   * This is an example of a chainable method.  That means that the return
   * value of this function is the MotionPlanner instance itself (`this`).  This lets
   * you do chained method calls like this:
   *
   * var myMotionPlanner = new MotionPlanner();
   * myMotionPlanner
   *   .chainableMethod()
   *   .chainableMethod();
   *
   * @return {MotionPlanner}
   */
  MotionPlanner.prototype.chainableMethod = function () {
    return this;
  };


  // DEBUG CODE
  //
  // With compiler directives, you can wrap code in a conditional check to
  // ensure that it does not get included in the compiled binaries.  This is
  // useful for exposing certain properties and methods that are needed during
  // development and testing, but should be private in the compiled binaries.


  if (DEBUG) {
    GLOBAL.corePrivateMethod = corePrivateMethod;
  }

}

// Your MotionPlanner may have many modules.  How you organize the modules is up to
// you, but generally speaking it's best if each module addresses a specific
// concern.  No module should need to know about the implementation details of
// any other module.

// Note:  You must name this function something unique.  If you end up
// copy/pasting this file, the last function defined will clobber the previous
// one.
function initMotionPlannerModule (context) {

  'use strict';

  var MotionPlanner = context.MotionPlanner;


  // A MotionPlanner module can do two things to the MotionPlanner Object:  It can extend
  // the prototype to add more methods, and it can add static properties.  This
  // is useful if your MotionPlanner needs helper methods.


  // PRIVATE MODULE CONSTANTS
  //


  var MODULE_CONSTANT = true;


  // PRIVATE MODULE METHODS
  //


  /**
   *  An example of a private method.  Feel free to remove this.
   */
  function modulePrivateMethod () {
    return;
  }


  // MotionPlanner STATIC PROPERTIES
  //


  /**
   * An example of a static MotionPlanner property.  This particular static property
   * is also an instantiable Object.
   * @constructor
   */
  MotionPlanner.MotionPlannerHelper = function () {
    return this;
  };


  // MotionPlanner PROTOTYPE EXTENSIONS
  //
  // A module can extend the prototype of the MotionPlanner Object.


  /**
   * An example of a prototype method.
   * @return {string}
   */
  MotionPlanner.prototype.alternateGetReadOnlyVar = function () {
    // Note that a module can access all of the MotionPlanner instance variables with
    // the `this` keyword.
    return this._readOnlyVar;
  };


  if (DEBUG) {
    // DEBUG CODE
    //
    // Each module can have its own debugging section.  They all get compiled
    // out of the binary.
  }

}

/*global initMotionPlannerCore initMotionPlannerModule */
var initMotionPlanner = function (context) {

  initMotionPlannerCore(context);
  initMotionPlannerModule(context);
  // Add a similar line as above for each module that you have.  If you have a
  // module named "Awesome Module," it should live in the file
  // "src/MotionPlanner.awesome-module.js" with a wrapper function named
  // "initAwesomeModule".  That function should then be invoked here with:
  //
  // initAwesomeModule(context);

  return context.MotionPlanner;
};


if (typeof define === 'function' && define.amd) {
  // Expose MotionPlanner as an AMD module if it's loaded with RequireJS or
  // similar.
  define(function () {
    return initMotionPlanner({});
  });
} else {
  // Load MotionPlanner normally (creating a MotionPlanner global) if not using an AMD
  // loader.
  initMotionPlanner(this);
}

} (this));

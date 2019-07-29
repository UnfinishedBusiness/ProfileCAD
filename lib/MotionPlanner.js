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
    this._Step_Scale = {x: 100, y: 100}; //This is the ammount of steps required to go one unit
    this.Motion = { dx: 0, dy: 0, sx: 0, sy: 0, err: 0, x_stg: 0, y_stg: 0};
    this.Callbacks = {};
    this.Callbacks.Step_X = function(dir){};
    this.Callbacks.Step_Y = function(dir){};
    this.CurrentPosition = {x: 0, y: 0};
    this.TargetPosition = {x: 0, y: 0};

    this.Stack = []; //Holds the pending moves. As moves are completed, they are shifted off the top of the stack

    return this;
  };
  MotionPlanner.prototype.motion_set_target = function(x, y)
  {
    this.TargetPosition.x = x;
    this.TargetPosition.y = y;
    this.Motion.dx = Math.abs(this.TargetPosition.x - this.CurrentPosition.x) * this._Step_Scale.x, this.Motion.sx = this.CurrentPosition.x < this.TargetPosition.x ? (1/this._Step_Scale.x) : -(1/this._Step_Scale.x);
    this.Motion.dy = Math.abs(this.TargetPosition.y - this.CurrentPosition.y) * this._Step_Scale.y, this.Motion.sy = this.CurrentPosition.y < this.TargetPosition.y ? (1/this._Step_Scale.y) : -(1/this._Step_Scale.y);
    this.Motion.err = (this.Motion.dx>this.Motion.dy ? this.Motion.dx : -this.Motion.dy)/2;
    this.Motion.x_stg = Math.abs(this.copy_obj(this.Motion.dx));
    this.Motion.y_stg = Math.abs(this.copy_obj(this.Motion.dy));
  }
  MotionPlanner.prototype.motion_tick = function()
  {
    if (this.Motion.y_stg > 0 || this.Motion.x_stg > 0)
    {
      console.log("X STG: " + this.Motion.x_stg, "Y STG: " + this.Motion.y_stg + " Current X: " + this.CurrentPosition.x + " Current Y: " + this.CurrentPosition.y);
      if (this.Motion.x_stg) if (this.Motion.err > -this.Motion.dx) { this.Motion.err -= this.Motion.dy; this.CurrentPosition.x += this.Motion.sx; this.Motion.x_stg--; this.Callbacks.Step_X(); }
      if (this.Motion.y_stg) if (this.Motion.err < this.Motion.dy) { this.Motion.err += this.Motion.dx; this.CurrentPosition.y += this.Motion.sy; this.Motion.y_stg--; this.Callbacks.Step_Y(); }
    }
    else
    {
      if (this.Stack.length > 0) //There are pending moves on the stack!
      {
        var next_move = this.Stack.shift();
        this.motion_set_target(next_move.x, next_move.y);
      }
      else //There are no pending moves on the stack!
      {

      }
      //console.log("End of move");
      //shift move from stack and traverse to next position
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

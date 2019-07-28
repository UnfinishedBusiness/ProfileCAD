/*! ProfileGraphics - Travis Gillin */
;(function (global) {

// Compiler directive for UglifyJS.  See ProfileGeometry.const.js for more info.
if (typeof DEBUG === 'undefined') {
  DEBUG = true;
}


// ProfileGeometry-GLOBAL CONSTANTS
//
// These constants are exposed to all ProfileGeometry modules.


// GLOBAL is a reference to the global Object.
var Fn = Function, GLOBAL = new Fn('return this')();


// ProfileGeometry-GLOBAL METHODS
//
// The methods here are exposed to all ProfileGeometry modules.  Because all of the
// source files are wrapped within a closure at build time, they are not
// exposed globally in the distributable binaries.


/**
 * A no-op function.  Useful for passing around as a default callback.
 */
function noop () { }


/**
 * Init wrapper for the core module.
 * @param {Object} The Object that the ProfileGeometry gets attached to in
 * ProfileGeometry.init.js.  If the ProfileGeometry was not loaded with an AMD loader such as
 * require.js, this is the global Object.
 */
function initProfileGeometryCore (context) {


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
   * This is the constructor for the ProfileGeometry Object.  Please rename it to
   * whatever your ProfileGeometry's name is.  Note that the constructor is also being
   * attached to the context that the ProfileGeometry was loaded in.
   * @param {Object} opt_config Contains any properties that should be used to
   * configure this instance of the ProfileGeometry.
   * @constructor
   */
  var ProfileGeometry = context.ProfileGeometry = function(opt_config) {

    opt_config = opt_config || {};

    // INSTANCE PROPERTY SETUP
    //
    // Your ProfileGeometry likely has some instance-specific properties.  The value of
    // these properties can depend on any number of things, such as properties
    // passed in via opt_config or global state.  Whatever the case, the values
    // should be set in this constructor.

    // Instance variables that have a leading underscore mean that they should
    // not be modified outside of the ProfileGeometry.  They can be freely modified
    // internally, however.  If an instance variable will likely be accessed
    // outside of the ProfileGeometry, consider making a public getter function for it.
    //this._readOnlyVar = 'read only';
    this.MakerJS = require('makerjs');
    this.Stack = [];

    return this;
  };

  // These methods define the public API.
   /**
   * Bind our Stack reference to renders Stack
   * @return {void}
   */
    ProfileGeometry.prototype.bindStack = function(stack) {
      this.Stack = stack;
    };
    /**
    * Return a copy of an object that arn't linked
    * @return {object}
    */
    ProfileGeometry.prototype.copy_obj = function(obj) {
      return JSON.parse(JSON.stringify(obj));
    };
    /**
    * Map a value betweeen ranges
    * @return {var}
    */
    ProfileGeometry.prototype.map = function(x, in_min, in_max, out_min, out_max)
    {
      return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }
    /**
    * Return a copied stack of entites that are currently selected in the order that they where
    * selected based on their selection timestamp
    * @return {object}
    */
    ProfileGeometry.prototype.getSelected = function()
    {
      var selected_stack = [];
      for (var x = 0; x < this.Stack.length; x++)
      {
        var part = this.Stack[x];
        if (part.hidden == false && part.internal == false)
        {
          for (var y = 0; y < part.entities.length; y++)
          {
            var entity = part.entities[y];
            if (entity.meta.selected == true)
            {
              selected_stack.push(this.copy_obj(entity));
            }
          }
        }
      }
      selected_stack.sort(function(a, b) {
      	return a.meta.selected_timestamp - b.meta.selected_timestamp;
  		});
      return selected_stack;
    }
    /**
    * Return a copied stack of entites that are currently selected in the order that they where
    * selected based on their selection timestamp
    * @return {object}
    */
    ProfileGeometry.prototype.getUnSelected = function()
    {
      var selected_stack = [];
      for (var x = 0; x < this.Stack.length; x++)
      {
        var part = this.Stack[x];
        if (part.hidden == false && part.internal == false)
        {
          for (var y = 0; y < part.entities.length; y++)
          {
            var entity = part.entities[y];
            if (entity.meta.selected == false)
            {
              selected_stack.push(this.copy_obj(entity));
            }
          }
        }
      }
      return selected_stack;
    }
    /**
    * Return a Stack of new entites that are offset 'dist' from the currently selected entities
    * @return {object}
    */
  	ProfileGeometry.prototype.offset_selected = function(dist)
  	{
  		var Selected = this.getSelected(); //Get Selected in order
  		var NewStack = [];
  		var distance = dist;
  		var direction = false; //Outside
  		if (distance < 0) direction = true;
  		var distance = Math.abs(distance);
  		var model = {};
  		model.models = {};
  		model.models.drawing = {};
  		model.models.drawing.paths = [];
  		for (var i = 0; i < Selected.length; i++)
  		{
  			model.models.drawing.paths.push(Selected[i]);
  		}
  		console.log(model);
  		var offset = this.MakerJS.model.outline(model, distance, 0, direction);
    	//var offset = this.MakerJS.model.expandPaths(model, distance, 0);
  		//console.log(offset);
  		for (var model_number in offset.models)
  		{
  			for (var pathname in offset.models[model_number].paths)
  			{
  				var offset_path = offset.models[model_number].paths[pathname];
  				NewStack.push(offset_path);
  			}
  		}

  		return NewStack;
  	}
  	ProfileGeometry.prototype.polarToCartesian = function(centerX, centerY, radius, angleInDegrees) {
  		var angleInRadians = (angleInDegrees) * Math.PI / 180.0;

  		return {
  			x: centerX + (radius * Math.cos(angleInRadians)),
  			y: centerY + (radius * Math.sin(angleInRadians))
  		};
  	}
  	ProfileGeometry.prototype.to_radians = function(degrees)
  	{
    	return degrees * Math.PI / 180;
  	}
  	ProfileGeometry.prototype.to_degrees = function(radians)
  	{
    	return radians * 180 / Math.PI;
  	}
  	ProfileGeometry.prototype.midpoint = function(p1, p2)
  	{
    	return {x: (p1.x+p2.x)/2, y:  (p1.y+p2.y)/2};
  	}
  	ProfileGeometry.prototype.distance = function(p1, p2)
  	{
  		return Math.sqrt( Math.pow((p1.x-p2.x), 2) + Math.pow((p1.y-p2.y), 2));
  	}
  	ProfileGeometry.prototype.get_line_angle = function(p1, p2)
  	{
  		var angle = this.to_degrees(Math.atan2(p1.y - p2.y, p1.x - p2.x));
  		angle += 180;
  		if (angle >= 360) angle -= 360;
  		return angle;
  	}
  	ProfileGeometry.prototype.get_all_intersections = function(e)
  	{
  		var Intersections = [];
  		for (var i = 0; i < this.Stack.length; i++)
  		{
        var part = this.Stack[i];
        if (part.hidden == false && part.internal == false)
        {
          for (var x = 0; x < part.entities.length; x++)
          {
            var entity = part.entities[x];
            var int = this.MakerJS.path.intersection(entity, e);
            if (int)
            {
              Intersections.push(int);
            }
          }
        }
  		}
  		return Intersections;
  	}
  	ProfileGeometry.prototype.get_intersection = function(l1, l2)
  	{
  		var int = this.MakerJS.path.intersection(l1, l2);
  		if (int)
  		{
  			return int.intersectionPoints[0];
  		}
  		else
  		{
  			return int;
  		}
  	}
  	ProfileGeometry.prototype.rotate_line = function(p1, p2, origin, angle)
  	{
  		var line1 = new this.MakerJS.paths.Line([p1.x, p1.y], [p2.x, p2.y]);
  		var paths = [this.MakerJS.path.rotate(line1, angle, [origin.x, origin.y])];
  		return paths[0];
  	}
  	ProfileGeometry.prototype.rotate_point = function(p, origin, angle)
  	{
  		var radians = (Math.PI / 180) * angle,
      cos = Math.cos(radians),
      sin = Math.sin(radians),
      nx = (cos * (p.x - origin.x)) + (sin * (p.y - origin.y)) + origin.x,
      ny = (cos * (p.y - origin.y)) - (sin * (p.x - origin.x)) + origin.y;
  		return { x: nx, y: ny };
  	}
  	ProfileGeometry.prototype.get_line_at_angle = function(origin, angle, distance)
  	{
  		var new_endpoint = { x: origin.x + (Math.abs(distance) * Math.cos(this.to_radians(angle))), y: origin.y + (Math.abs(distance) * Math.sin(this.to_radians(angle))) };
    	return { type: 'line', origin: [origin.x, origin.y], end: [new_endpoint.x, new_endpoint.y] };
  	}
  	ProfileGeometry.prototype.mirror_point_about_line = function(p, p0, p1)
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
  	ProfileGeometry.prototype.CircleCenterFromThreePoints = function(A,B,C)
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
   * value of this function is the ProfileGeometry instance itself (`this`).  This lets
   * you do chained method calls like this:
   *
   * var myProfileGeometry = new ProfileGeometry();
   * myProfileGeometry
   *   .chainableMethod()
   *   .chainableMethod();
   *
   * @return {ProfileGeometry}
   */
  ProfileGeometry.prototype.chainableMethod = function () {
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

// Your ProfileGeometry may have many modules.  How you organize the modules is up to
// you, but generally speaking it's best if each module addresses a specific
// concern.  No module should need to know about the implementation details of
// any other module.

// Note:  You must name this function something unique.  If you end up
// copy/pasting this file, the last function defined will clobber the previous
// one.
function initProfileGeometryModule (context) {

  'use strict';

  var ProfileGeometry = context.ProfileGeometry;


  // A ProfileGeometry module can do two things to the ProfileGeometry Object:  It can extend
  // the prototype to add more methods, and it can add static properties.  This
  // is useful if your ProfileGeometry needs helper methods.


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


  // ProfileGeometry STATIC PROPERTIES
  //


  /**
   * An example of a static ProfileGeometry property.  This particular static property
   * is also an instantiable Object.
   * @constructor
   */
  ProfileGeometry.ProfileGeometryHelper = function () {
    return this;
  };


  // ProfileGeometry PROTOTYPE EXTENSIONS
  //
  // A module can extend the prototype of the ProfileGeometry Object.


  /**
   * An example of a prototype method.
   * @return {string}
   */
  ProfileGeometry.prototype.alternateGetReadOnlyVar = function () {
    // Note that a module can access all of the ProfileGeometry instance variables with
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

/*global initProfileGeometryCore initProfileGeometryModule */
var initProfileGeometry = function (context) {

  initProfileGeometryCore(context);
  initProfileGeometryModule(context);
  // Add a similar line as above for each module that you have.  If you have a
  // module named "Awesome Module," it should live in the file
  // "src/ProfileGeometry.awesome-module.js" with a wrapper function named
  // "initAwesomeModule".  That function should then be invoked here with:
  //
  // initAwesomeModule(context);

  return context.ProfileGeometry;
};


if (typeof define === 'function' && define.amd) {
  // Expose ProfileGeometry as an AMD module if it's loaded with RequireJS or
  // similar.
  define(function () {
    return initProfileGeometry({});
  });
} else {
  // Load ProfileGeometry normally (creating a ProfileGeometry global) if not using an AMD
  // loader.
  initProfileGeometry(this);
}

} (this));

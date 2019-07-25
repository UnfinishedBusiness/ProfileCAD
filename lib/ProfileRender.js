/*! ProfileGraphics - Travis Gillin */
;(function (global) {

// Compiler directive for UglifyJS.  See ProfileRender.const.js for more info.
if (typeof DEBUG === 'undefined') {
  DEBUG = true;
}


// ProfileRender-GLOBAL CONSTANTS
//
// These constants are exposed to all ProfileRender modules.


// GLOBAL is a reference to the global Object.
var Fn = Function, GLOBAL = new Fn('return this')();


// ProfileRender-GLOBAL METHODS
//
// The methods here are exposed to all ProfileRender modules.  Because all of the
// source files are wrapped within a closure at build time, they are not
// exposed globally in the distributable binaries.


/**
 * A no-op function.  Useful for passing around as a default callback.
 */
function noop () { }


/**
 * Init wrapper for the core module.
 * @param {Object} The Object that the ProfileRender gets attached to in
 * ProfileRender.init.js.  If the ProfileRender was not loaded with an AMD loader such as
 * require.js, this is the global Object.
 */
function initProfileRenderCore (context) {


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
   * This is the constructor for the ProfileRender Object.  Please rename it to
   * whatever your ProfileRender's name is.  Note that the constructor is also being
   * attached to the context that the ProfileRender was loaded in.
   * @param {Object} opt_config Contains any properties that should be used to
   * configure this instance of the ProfileRender.
   * @constructor
   */
  var ProfileRender = context.ProfileRender = function (opt_config) {

    opt_config = opt_config || {};

    // INSTANCE PROPERTY SETUP
    //
    // Your ProfileRender likely has some instance-specific properties.  The value of
    // these properties can depend on any number of things, such as properties
    // passed in via opt_config or global state.  Whatever the case, the values
    // should be set in this constructor.

    // Instance variables that have a leading underscore mean that they should
    // not be modified outside of the ProfileRender.  They can be freely modified
    // internally, however.  If an instance variable will likely be accessed
    // outside of the ProfileRender, consider making a public getter function for it.
    //this._readOnlyVar = 'read only';

    /* External Library Dependancies */
    this.geometry = null;

    /* Drafting Colors */
    this._colorRed = 0xf04923;
    this._colorBlue = 0x0101f9;
    this._colorBlack = 0x271e17;
    this._colorGreen = 0x36a738;
    this._colorOrange = 0xf2a343;
    this._colorPurple = 0x5025833;
    this._colorWhite = 0xf9f6f0;
    /* End Drafting Colors */

    /* Render Parameters */
    this._mouseOverPadding = 0.08;
    this._near = 0.001;
    this._far = 5000;
    this._crosshair_size = 0.05;
    this._selectedColor = this._colorGreen;
    this._mouse_overColor = this._colorBlue;
    this._defaultMeta = {color: "white", selected: false, selected_timestamp: null, style: "solid", mouse_over: false, snap_points: []};
    this._crosshairMeta = {color: "red", selected: false, selected_timestamp: null, style: "solid", mouse_over: false, snap_points: []};



    this._dirClockwise = true;
    this._dirCounterClockwise = false;
    // Instance variables that do not have an underscore prepended are
    // considered to be part of the ProfileRender's public API.  External code may
    // change the value of these variables freely.
    //this.readAndWrite = 'read and write';
    this.renderer;
    this.scene;
    this.camera;
    this.controls;
    this.mousePosition = {x: 0, y: 0};
    this.lookAt = {x: 0, y: 0, z: 0};
    this.mouse_over_check = function(){};

    /* This stack stores all parts and it's childrens entities. When a child endities are modified, the updateRender flag must be set to sync it will the scene */
    this.Stack = [
      { part_name: "OriginCrosshair", offset: [0, 0, 0], entities: [
        { type: "line", origin: [0, 0, 0], end: [this._crosshair_size, 0, 0], meta: this.copy_obj(this._crosshairMeta) },
        { type: "line", origin: [0, 0, 0], end: [-this._crosshair_size, 0, 0], meta: this.copy_obj(this._crosshairMeta) },
        { type: "line", origin: [0, 0, 0], end: [0, this._crosshair_size, 0], meta: this.copy_obj(this._crosshairMeta) },
        { type: "line", origin: [0, 0, 0], end: [0, -this._crosshair_size, 0], meta: this.copy_obj(this._crosshairMeta) },
        { type: "line", origin: [0, 0, 0], end: [0, 0, this._crosshair_size], meta: this.copy_obj(this._crosshairMeta) },
        { type: "line", origin: [0, 0, 0], end: [0, 0, -this._crosshair_size], meta: this.copy_obj(this._crosshairMeta) },
      ], updateRender: true, hidden: false },
    ];

    return this;
  };


  // ProfileRender PROTOTYPE METHODS
  //
  // These methods define the public API.
  /**
   * Return a copy of an object that arn't linked
   * @return {object}
   */
    ProfileRender.prototype.copy_obj = function (obj) {
      return JSON.parse(JSON.stringify(obj));
    };
    /**
     * Update parts rendering
     * @return {void}
     */
    ProfileRender.prototype.updatePart = function (part) {
      part.updateRender = true;
      this.build_scene();
    };
    /**
     * Iterate Stack and build mouse_over meta data and flag renderUpdate fro build scene to update if needed. For entity manipulation
     * @return {void}
     */
    ProfileRender.prototype.entity_mouse_interaction = function () {

      for (var x = 0; x < this.Stack.length; x++)
      {
        var part = this.Stack[x];
        if (part.hidden == false)
        {
          for (var y = 0; y < part.entities.length; y++)
          {
            var entity = part.entities[y];
            if (entity.type == "line")
            {
              var z_factor = this.camera.position.z * 0.1;
              var angle = this.geometry.get_line_angle({x: entity.origin[0], y: entity.origin[1] }, {x: entity.end[0], y: entity.end[1] }) + 90;
              if (angle >= 360) angle -= 360;
              var line1 = this.geometry.get_line_at_angle({x: this.mousePosition.x, y: this.mousePosition.y}, angle, (z_factor * this._mouseOverPadding) / 2);
              var line2 = this.geometry.get_line_at_angle({x: this.mousePosition.x, y: this.mousePosition.y}, angle + 180, (z_factor * this._mouseOverPadding) / 2);

              var actual_line =  { type: 'line', origin: line1.end, end: line2.end };
              let int = this.geometry.get_intersection(actual_line, entity);
              if (int)
              {
                entity.meta.mouse_over = true;
                this.updatePart(part);
                /*this.metaMouseOver(x, true);
                var midpoint = this.midpoint({x: this.Stack[x].origin[0], y: this.Stack[x].origin[1]}, {x: this.Stack[x].end[0], y: this.Stack[x].end[1]});
                var intersections = this.get_all_intersections(this.Stack[x]);
                var snap_points = [ {x: this.Stack[x].origin[0], y: this.Stack[x].origin[1]}, {x: this.Stack[x].end[0], y: this.Stack[x].end[1]}, { x: midpoint.x, y: midpoint.y } ];
                for(var y = 0; y < intersections.length; y++)
                {
                  if (intersections[y].intersectionPoints !== undefined)
                  {
                    var points = intersections[y].intersectionPoints
                    for (var z = 0; z < points.length; z++)
                    {
                      snap_points.push({ x: points[z][0], y: points[z][1]});
                    }
                  }

                }*/
              }
              else
              {
                if (entity.meta.mouse_over == true) //If mouse was previosly over and are not not then switch back and trigger re-renders
                {
                  entity.meta.mouse_over = false;
                  this.updatePart(part);
                }
              }
            }
            else if (entity.type == "circle")
            {
              if (this.geometry.distance(this.mousePosition, {x: entity.origin[0], y: entity.origin[1] } ) < entity.radius + ((z_factor * this._mouseOverPadding)) &&
      						this.geometry.distance(this.mousePosition, {x: entity.origin[0], y: entity.origin[1] } ) > entity.radius - ((z_factor * this._mouseOverPadding)))
      				{
                entity.meta.mouse_over = true;
                this.updatePart(part);
      					//this.geometry.metaSnapPoints(x, [ {x: this.Stack[x].origin[0], y: this.Stack[x].origin[1]} ]);
      				}
      				else
      				{
                if (entity.meta.mouse_over == true) //If mouse was previosly over and are not not then switch back and trigger re-renders
                {
                  entity.meta.mouse_over = false;
                  this.updatePart(part);
                }
      				}
            }
            else if (entity.type == "arc")
            {
              var drag_line =  { type: 'line', origin: entity.origin, end: [ this.mousePosition.x, this.mousePosition.y] };
              let int = this.geometry.get_intersection(drag_line, entity);
              if (int)
              {
                if (this.geometry.distance(this.mousePosition, { x: entity.origin[0], y: entity.origin[1]} ) < entity.radius + (z_factor * this._mouseOverPadding) &&
                  this.geometry.distance(this.mousePosition, { x: entity.origin[0], y: entity.origin[1]} ) > entity.radius - (z_factor * this._mouseOverPadding))
                {
                  entity.meta.mouse_over = true;
                  this.updatePart(part);
                  var start = this.geometry.polarToCartesian(entity.origin[0], entity.origin[1], entity.radius, entity.startAngle);
                  var end = this.geometry.polarToCartesian(entity.origin[0], entity.origin[1], entity.radius, entity.endAngle);
                  //this.metaSnapPoints(x, [ {x: this.Stack[x].origin[0], y: this.Stack[x].origin[1]}, start, end ]);
                }
                else
                {
                  if (entity.meta.mouse_over == true) //If mouse was previosly over and are not not then switch back and trigger re-renders
                  {
                    entity.meta.mouse_over = false;
                    this.updatePart(part);
                  }
                }
              }
              else
              {
                if (entity.meta.mouse_over == true) //If mouse was previosly over and are not not then switch back and trigger re-renders
                {
                  entity.meta.mouse_over = false;
                  this.updatePart(part);
                }
              }
            }
          }
        }
      }
    }
  /**
   * Iterate Stack and build scene. When updateRender flag is set, update entire parts scene
   * @return {void}
   */
  ProfileRender.prototype.build_scene = function () {
    for (var x = 0; x < this.Stack.length; x++)
    {
      var part = this.Stack[x];
      if (part.updateRender == true)
      {
        part.updateRender = false;
        this.removePartFromScene(part.part_name); //Remove all of parts entities from scene, then re-render new/changed ones
        if (part.hidden == false)
        {
          for (var y = 0; y < part.entities.length; y++)
          {
            var entity = part.entities[y];
            if (entity.type == "line")
            {
              entity.sceneObject = this.drawLine(part, entity);
            }
            if (entity.type == "arc")
            {
              entity.sceneObject = this.drawArc(part, entity);
            }
            if (entity.type == "circle")
            {
              var circle = this.copy_obj(entity);
              circle.startAngle = 0;
              circle.endAngle = 0;
              entity.sceneObject = this.drawArc(part, circle);
            }
          }
        }
      }
    }
    render.renderer.render( render.scene, render.camera );
  };
  /**
   * Iterate Stack and build scene. When updateRender flag is set, update entire parts scene
   * @return {void}
   */
  ProfileRender.prototype.removePartFromScene = function (part_name) {
    var enties_to_remove = [];
    for (var x = 0; x < this.scene.children.length; x++)
    {
      if (this.scene.children[x].part_name == part_name)
      {
        enties_to_remove.push(this.scene.children[x]);
      }
    }
    while(enties_to_remove.length > 0)
    {
      var entity = enties_to_remove.shift();
      this.scene.remove(entity);
    }
  };
  /**
   * Get Hex color from color name as string
   * @return {void}
   */
  ProfileRender.prototype.get_color_hex= function (color) {
    if (color == "red") return this._colorRed;
    if (color == "blue") return this._colorBlue;
    if (color == "black") return this._colorBlack;
    if (color == "green") return this._colorGreen;
    if (color == "orange") return this._colorOrange;
    if (color == "purple") return this._colorPurple ;
    if (color == "white") return this._colorWhite;
  };
  /**
   * Init Three.js render and setup our scene
   * @return {void}
   */
  ProfileRender.prototype.init = function () {
    this.renderer = new THREE.WebGLRenderer();
    this.renderer.setSize( window.innerWidth, window.innerHeight );
    document.body.appendChild( this.renderer.domElement );
    this.camera = new THREE.PerspectiveCamera( 45, window.innerWidth / window.innerHeight, this._near, this._far );
    //this.camera = new THREE.OrthographicCamera( window.innerWidth / window.innerHeight, this._near, this._far );
    this.camera.position.set( 0, 0, 100 );
    this.camera.lookAt( this.lookAt.x, this.lookAt.y, this.lookAt.z );
    this.controls = new THREE.OrbitControls (this.camera, this.renderer.domElement);
    this.controls.enablePan = false;
    //this.controls.enableRotate = false;
    this.controls.mouseButtons = { LEFT: 1, MIDDLE: 0, RIGHT: 2 };
    this.controls.enableZoom = false;
    this.scene = new THREE.Scene();
    this.renderer.render( this.scene, this.camera );
    var that = this;
    setInterval(function() { that.build_scene(); }, 100);
    this.renderer.domElement.addEventListener("mousemove", function(e){ that.onMouseMotion(e); });
    this.renderer.domElement.addEventListener("mousedown", function(e){ that.onMouseClick(e); });
    this.renderer.domElement.addEventListener("wheel", function(e){ that.onMouseWheel(e); });
    this.geometry = new ProfileGeometry();
    this.geometry.bindStack(this.Stack);
    this.mouse_over_check = this.entity_mouse_interaction;
    return;
  };

  /**
   * Push a line to the scene
   * @return {THREE.Line}
   */
  ProfileRender.prototype.drawLine = function (part, line) {
    var color = this.get_color_hex(line.meta.color);
    if (line.meta.selected == true)
    {
      color = this._selectedColor;
    }
    if (line.meta.mouse_over == true)
    {
      color = this._mouse_overColor;
    }
    var material = new THREE.LineBasicMaterial( { color: color } );
    var geometry = new THREE.Geometry();
    geometry.vertices.push(new THREE.Vector3( part.offset[0] + line.origin[0], part.offset[1] + line.origin[1], part.offset[2] + line.origin[2]) );
    geometry.vertices.push(new THREE.Vector3( part.offset[0] + line.end[0], part.offset[1] + line.end[1], part.offset[2] + line.end[2]) );
    var line = new THREE.Line( geometry, material );
    line.part_name = part.part_name; //Store the part name in the scene object
    this.scene.add( line );
    return line;
  };
  /**
   * Push a arc to the scene
   * @return {THREE.Line}
   */
  ProfileRender.prototype.drawArc = function (part, arc) {
    var curve = new THREE.EllipseCurve(
      part.offset[0] + arc.origin[0], part.offset[1] + arc.origin[1],             // ax, aY
      arc.radius, arc.radius,            // xRadius, yRadius
      this.to_radians(arc.startAngle), this.to_radians(arc.endAngle), // aStartAngle, aEndAngle
      arc.direction             // true = aClockwise
    );
    var points = curve.getSpacedPoints( 20 );
    var path = new THREE.Path();
    var geometry = new THREE.Geometry().setFromPoints( points );
    var color = this.get_color_hex(arc.meta.color);
    if (arc.meta.selected == true)
    {
      color = this._selectedColor;
    }
    if (arc.meta.mouse_over == true)
    {
      color = this._mouse_overColor;
    }
    var material = new THREE.LineBasicMaterial( { color: color } );
    var line = new THREE.Line( geometry, material );
    line.part_name = part.part_name; //Store the part name in the scene object
    this.scene.add( line );
    return line;
  };
  /** Event Handler functions **/
  ProfileRender.prototype.onMouseMotion = function (e){
    var vec = new THREE.Vector3(); // create once and reuse
    var pos = new THREE.Vector3(); // create once and reuse

    vec.set(
        ( e.clientX / window.innerWidth ) * 2 - 1,
        - ( e.clientY / window.innerHeight ) * 2 + 1,
        0.5 );
    vec.unproject( this.camera );
    vec.sub( this.camera.position ).normalize();
    var distance = - this.camera.position.z / vec.z;
    pos.copy( this.camera.position ).add( vec.multiplyScalar( distance ) );
    this.mousePosition = pos;
    this.mouse_over_check();
    //console.log(pos);
	}
  ProfileRender.prototype.onMouseClick = function (e){
    this.onMouseMotion(e); //Update mouse position in world cordinates to be the precise location of the click
    for (var x = 0; x < this.Stack.length; x++)
    {
      var part = this.Stack[x];
      if (part.hidden == false)
      {
        for (var y = 0; y < part.entities.length; y++)
        {
          var entity = part.entities[y];
          if (entity.meta.mouse_over == true) //If we currently are not selected, select it and set the selection timestamp for selection ordering
          {
            if (entity.meta.selected == true) //If we are currently selected, un-select it
            {
              entity.meta.mouse_over = false;
              entity.meta.selected = false;
              entity.meta.selected_timestamp = null;
              this.updatePart(part);
            }
            else //Select the entity that the mouse is currently over
            {
              entity.meta.mouse_over = false;
              entity.meta.selected = true;
              entity.meta.selected_timestamp = Date.now();
              this.updatePart(part);
            }
          }
        }
      }
    }
	}
  ProfileRender.prototype.onMouseWheel = function (e){
    this.onMouseMotion(e);
    var factor = this.camera.position.z * 0.1;
    //console.log("factor: " + factor);
    this.scale_originCrosshair(factor);
    var mX = (event.clientX / window.innerWidth) * 2 - 1;
    var mY = -(event.clientY / window.innerHeight) * 2 + 1;
    var vector = new THREE.Vector3(mX, mY, 0.1);
    vector.unproject(this.camera);
    vector.sub(this.camera.position);
    if (e.deltaY < 0) {
       this.camera.position.addVectors(this.camera.position, vector.setLength(factor));
       this.controls.target.addVectors(this.controls.target, vector.setLength(factor));
    } else {
      this.camera.position.subVectors(this.camera.position, vector.setLength(factor));
      this.controls.target.subVectors(this.controls.target, vector.setLength(factor));
    }
	}
  /** Simple Helper functions **/
  ProfileRender.prototype.scale_originCrosshair = function (scale){
    for (var x = 0; x < this.Stack.length; x++)
    {
      if (this.Stack[x].part_name == "OriginCrosshair")
      {
        this.Stack[x].entities = [
        { type: "line", origin: [0, 0, 0], end: [(scale * this._crosshair_size), 0, 0], meta: this.copy_obj(this._crosshairMeta) },
        { type: "line", origin: [0, 0, 0], end: [-(scale * this._crosshair_size), 0, 0], meta: this.copy_obj(this._crosshairMeta) },
        { type: "line", origin: [0, 0, 0], end: [0, (scale * this._crosshair_size), 0], meta: this.copy_obj(this._crosshairMeta) },
        { type: "line", origin: [0, 0, 0], end: [0, -(scale * this._crosshair_size), 0], meta: this.copy_obj(this._crosshairMeta) },
        { type: "line", origin: [0, 0, 0], end: [0, 0, (scale * this._crosshair_size)], meta: this.copy_obj(this._crosshairMeta) },
        { type: "line", origin: [0, 0, 0], end: [0, 0, -(scale * this._crosshair_size)], meta: this.copy_obj(this._crosshairMeta) },
        ];
        this.Stack[x].updateRender = true;
        return;
      }
    }
	}
  ProfileRender.prototype.to_radians = function (degrees){
  	return degrees * Math.PI / 180;
	}
	ProfileRender.prototype.to_degrees = function (radians){
  	return radians * 180 / Math.PI;
	}
  ProfileRender.prototype.rotate_point = function(p, origin, angle)
	{
		var radians = (Math.PI / 180) * angle,
    cos = Math.cos(radians),
    sin = Math.sin(radians),
    nx = (cos * (p.x - origin.x)) + (sin * (p.y - origin.y)) + origin.x,
    ny = (cos * (p.y - origin.y)) - (sin * (p.x - origin.x)) + origin.y;
		return { x: nx, y: ny };
	}
  ProfileRender.prototype.map = function(x, in_min, in_max, out_min, out_max)
  {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  }

  /**
   * This is an example of a chainable method.  That means that the return
   * value of this function is the ProfileRender instance itself (`this`).  This lets
   * you do chained method calls like this:
   *
   * var myProfileRender = new ProfileRender();
   * myProfileRender
   *   .chainableMethod()
   *   .chainableMethod();
   *
   * @return {ProfileRender}
   */
  ProfileRender.prototype.chainableMethod = function () {
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

// Your ProfileRender may have many modules.  How you organize the modules is up to
// you, but generally speaking it's best if each module addresses a specific
// concern.  No module should need to know about the implementation details of
// any other module.

// Note:  You must name this function something unique.  If you end up
// copy/pasting this file, the last function defined will clobber the previous
// one.
function initProfileRenderModule (context) {

  'use strict';

  var ProfileRender = context.ProfileRender;


  // A ProfileRender module can do two things to the ProfileRender Object:  It can extend
  // the prototype to add more methods, and it can add static properties.  This
  // is useful if your ProfileRender needs helper methods.


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


  // ProfileRender STATIC PROPERTIES
  //


  /**
   * An example of a static ProfileRender property.  This particular static property
   * is also an instantiable Object.
   * @constructor
   */
  ProfileRender.ProfileRenderHelper = function () {
    return this;
  };


  // ProfileRender PROTOTYPE EXTENSIONS
  //
  // A module can extend the prototype of the ProfileRender Object.


  /**
   * An example of a prototype method.
   * @return {string}
   */
  ProfileRender.prototype.alternateGetReadOnlyVar = function () {
    // Note that a module can access all of the ProfileRender instance variables with
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

/*global initProfileRenderCore initProfileRenderModule */
var initProfileRender = function (context) {

  initProfileRenderCore(context);
  initProfileRenderModule(context);
  // Add a similar line as above for each module that you have.  If you have a
  // module named "Awesome Module," it should live in the file
  // "src/ProfileRender.awesome-module.js" with a wrapper function named
  // "initAwesomeModule".  That function should then be invoked here with:
  //
  // initAwesomeModule(context);

  return context.ProfileRender;
};


if (typeof define === 'function' && define.amd) {
  // Expose ProfileRender as an AMD module if it's loaded with RequireJS or
  // similar.
  define(function () {
    return initProfileRender({});
  });
} else {
  // Load ProfileRender normally (creating a ProfileRender global) if not using an AMD
  // loader.
  initProfileRender(this);
}

} (this));

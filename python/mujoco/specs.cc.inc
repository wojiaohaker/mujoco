
  // mjOption
mjOption.def_property("timestep",[](raw::MjOption& self) -> mjtNum {
        return self.timestep;
      },[](raw::MjOption& self, mjtNum timestep) {
        self.timestep = timestep;
      });
mjOption.def_property("apirate",[](raw::MjOption& self) -> mjtNum {
        return self.apirate;
      },[](raw::MjOption& self, mjtNum apirate) {
        self.apirate = apirate;
      });
mjOption.def_property("impratio",[](raw::MjOption& self) -> mjtNum {
        return self.impratio;
      },[](raw::MjOption& self, mjtNum impratio) {
        self.impratio = impratio;
      });
mjOption.def_property("tolerance",[](raw::MjOption& self) -> mjtNum {
        return self.tolerance;
      },[](raw::MjOption& self, mjtNum tolerance) {
        self.tolerance = tolerance;
      });
mjOption.def_property("ls_tolerance",[](raw::MjOption& self) -> mjtNum {
        return self.ls_tolerance;
      },[](raw::MjOption& self, mjtNum ls_tolerance) {
        self.ls_tolerance = ls_tolerance;
      });
mjOption.def_property("noslip_tolerance",[](raw::MjOption& self) -> mjtNum {
        return self.noslip_tolerance;
      },[](raw::MjOption& self, mjtNum noslip_tolerance) {
        self.noslip_tolerance = noslip_tolerance;
      });
mjOption.def_property("ccd_tolerance",[](raw::MjOption& self) -> mjtNum {
        return self.ccd_tolerance;
      },[](raw::MjOption& self, mjtNum ccd_tolerance) {
        self.ccd_tolerance = ccd_tolerance;
      });
  mjOption.def_property(
      "gravity",
      [](raw::MjOption& self) -> MjDouble3 {
        return MjDouble3(self.gravity);
    },
      [](raw::MjOption& self, MjDoubleRef3 gravity) {
        MjDouble3(self.gravity) = gravity;
    }, py::return_value_policy::reference_internal);
  mjOption.def_property(
      "wind",
      [](raw::MjOption& self) -> MjDouble3 {
        return MjDouble3(self.wind);
    },
      [](raw::MjOption& self, MjDoubleRef3 wind) {
        MjDouble3(self.wind) = wind;
    }, py::return_value_policy::reference_internal);
  mjOption.def_property(
      "magnetic",
      [](raw::MjOption& self) -> MjDouble3 {
        return MjDouble3(self.magnetic);
    },
      [](raw::MjOption& self, MjDoubleRef3 magnetic) {
        MjDouble3(self.magnetic) = magnetic;
    }, py::return_value_policy::reference_internal);
mjOption.def_property("density",[](raw::MjOption& self) -> mjtNum {
        return self.density;
      },[](raw::MjOption& self, mjtNum density) {
        self.density = density;
      });
mjOption.def_property("viscosity",[](raw::MjOption& self) -> mjtNum {
        return self.viscosity;
      },[](raw::MjOption& self, mjtNum viscosity) {
        self.viscosity = viscosity;
      });
mjOption.def_property("o_margin",[](raw::MjOption& self) -> mjtNum {
        return self.o_margin;
      },[](raw::MjOption& self, mjtNum o_margin) {
        self.o_margin = o_margin;
      });
  mjOption.def_property(
      "o_solref",
      [](raw::MjOption& self) -> MjDouble2 {
        return MjDouble2(self.o_solref);
    },
      [](raw::MjOption& self, MjDoubleRef2 o_solref) {
        MjDouble2(self.o_solref) = o_solref;
    }, py::return_value_policy::reference_internal);
  mjOption.def_property(
      "o_solimp",
      [](raw::MjOption& self) -> MjDouble5 {
        return MjDouble5(self.o_solimp);
    },
      [](raw::MjOption& self, MjDoubleRef5 o_solimp) {
        MjDouble5(self.o_solimp) = o_solimp;
    }, py::return_value_policy::reference_internal);
  mjOption.def_property(
      "o_friction",
      [](raw::MjOption& self) -> MjDouble5 {
        return MjDouble5(self.o_friction);
    },
      [](raw::MjOption& self, MjDoubleRef5 o_friction) {
        MjDouble5(self.o_friction) = o_friction;
    }, py::return_value_policy::reference_internal);
mjOption.def_property("integrator",[](raw::MjOption& self) -> int {
        return self.integrator;
      },[](raw::MjOption& self, int integrator) {
        self.integrator = integrator;
      });
mjOption.def_property("cone",[](raw::MjOption& self) -> int {
        return self.cone;
      },[](raw::MjOption& self, int cone) {
        self.cone = cone;
      });
mjOption.def_property("jacobian",[](raw::MjOption& self) -> int {
        return self.jacobian;
      },[](raw::MjOption& self, int jacobian) {
        self.jacobian = jacobian;
      });
mjOption.def_property("solver",[](raw::MjOption& self) -> int {
        return self.solver;
      },[](raw::MjOption& self, int solver) {
        self.solver = solver;
      });
mjOption.def_property("iterations",[](raw::MjOption& self) -> int {
        return self.iterations;
      },[](raw::MjOption& self, int iterations) {
        self.iterations = iterations;
      });
mjOption.def_property("ls_iterations",[](raw::MjOption& self) -> int {
        return self.ls_iterations;
      },[](raw::MjOption& self, int ls_iterations) {
        self.ls_iterations = ls_iterations;
      });
mjOption.def_property("noslip_iterations",[](raw::MjOption& self) -> int {
        return self.noslip_iterations;
      },[](raw::MjOption& self, int noslip_iterations) {
        self.noslip_iterations = noslip_iterations;
      });
mjOption.def_property("ccd_iterations",[](raw::MjOption& self) -> int {
        return self.ccd_iterations;
      },[](raw::MjOption& self, int ccd_iterations) {
        self.ccd_iterations = ccd_iterations;
      });
mjOption.def_property("disableflags",[](raw::MjOption& self) -> int {
        return self.disableflags;
      },[](raw::MjOption& self, int disableflags) {
        self.disableflags = disableflags;
      });
mjOption.def_property("enableflags",[](raw::MjOption& self) -> int {
        return self.enableflags;
      },[](raw::MjOption& self, int enableflags) {
        self.enableflags = enableflags;
      });
mjOption.def_property("disableactuator",[](raw::MjOption& self) -> int {
        return self.disableactuator;
      },[](raw::MjOption& self, int disableactuator) {
        self.disableactuator = disableactuator;
      });
mjOption.def_property("sdf_initpoints",[](raw::MjOption& self) -> int {
        return self.sdf_initpoints;
      },[](raw::MjOption& self, int sdf_initpoints) {
        self.sdf_initpoints = sdf_initpoints;
      });
mjOption.def_property("sdf_iterations",[](raw::MjOption& self) -> int {
        return self.sdf_iterations;
      },[](raw::MjOption& self, int sdf_iterations) {
        self.sdf_iterations = sdf_iterations;
      });

  // mjVisual
mjVisual.def_property("global",[](raw::MjVisual& self) -> raw::MjVisualGlobal& {
        return self.global;
      },[](raw::MjVisual& self, raw::MjVisualGlobal& global) {
        self.global = global;
      },py::return_value_policy::reference_internal);
mjVisual.def_property("quality",[](raw::MjVisual& self) -> raw::MjVisualQuality& {
        return self.quality;
      },[](raw::MjVisual& self, raw::MjVisualQuality& quality) {
        self.quality = quality;
      },py::return_value_policy::reference_internal);
  mjVisualHeadlight.def_property(
      "ambient",
      [](raw::MjVisualHeadlight& self) -> MjFloat3 {
        return MjFloat3(self.ambient);
    },
      [](raw::MjVisualHeadlight& self, MjFloatRef3 ambient) {
        MjFloat3(self.ambient) = ambient;
    }, py::return_value_policy::reference_internal);  mjVisualHeadlight.def_property(
      "diffuse",
      [](raw::MjVisualHeadlight& self) -> MjFloat3 {
        return MjFloat3(self.diffuse);
    },
      [](raw::MjVisualHeadlight& self, MjFloatRef3 diffuse) {
        MjFloat3(self.diffuse) = diffuse;
    }, py::return_value_policy::reference_internal);  mjVisualHeadlight.def_property(
      "specular",
      [](raw::MjVisualHeadlight& self) -> MjFloat3 {
        return MjFloat3(self.specular);
    },
      [](raw::MjVisualHeadlight& self, MjFloatRef3 specular) {
        MjFloat3(self.specular) = specular;
    }, py::return_value_policy::reference_internal);mjVisualHeadlight.def_property("active",[](raw::MjVisualHeadlight& self) -> int {
        return self.active;
      },[](raw::MjVisualHeadlight& self, int active) {
        self.active = active;
      });mjVisual.def_property("headlight",[](raw::MjVisual& self) -> raw::MjVisualHeadlight& {
        return self.headlight;
      },[](raw::MjVisual& self, raw::MjVisualHeadlight& headlight) {
        self.headlight = headlight;
      },py::return_value_policy::reference_internal);
mjVisual.def_property("map",[](raw::MjVisual& self) -> raw::MjVisualMap& {
        return self.map;
      },[](raw::MjVisual& self, raw::MjVisualMap& map) {
        self.map = map;
      },py::return_value_policy::reference_internal);
mjVisual.def_property("scale",[](raw::MjVisual& self) -> raw::MjVisualScale& {
        return self.scale;
      },[](raw::MjVisual& self, raw::MjVisualScale& scale) {
        self.scale = scale;
      },py::return_value_policy::reference_internal);
  mjVisualRgba.def_property(
      "fog",
      [](raw::MjVisualRgba& self) -> MjFloat4 {
        return MjFloat4(self.fog);
    },
      [](raw::MjVisualRgba& self, MjFloatRef4 fog) {
        MjFloat4(self.fog) = fog;
    }, py::return_value_policy::reference_internal);  mjVisualRgba.def_property(
      "haze",
      [](raw::MjVisualRgba& self) -> MjFloat4 {
        return MjFloat4(self.haze);
    },
      [](raw::MjVisualRgba& self, MjFloatRef4 haze) {
        MjFloat4(self.haze) = haze;
    }, py::return_value_policy::reference_internal);  mjVisualRgba.def_property(
      "force",
      [](raw::MjVisualRgba& self) -> MjFloat4 {
        return MjFloat4(self.force);
    },
      [](raw::MjVisualRgba& self, MjFloatRef4 force) {
        MjFloat4(self.force) = force;
    }, py::return_value_policy::reference_internal);  mjVisualRgba.def_property(
      "inertia",
      [](raw::MjVisualRgba& self) -> MjFloat4 {
        return MjFloat4(self.inertia);
    },
      [](raw::MjVisualRgba& self, MjFloatRef4 inertia) {
        MjFloat4(self.inertia) = inertia;
    }, py::return_value_policy::reference_internal);  mjVisualRgba.def_property(
      "joint",
      [](raw::MjVisualRgba& self) -> MjFloat4 {
        return MjFloat4(self.joint);
    },
      [](raw::MjVisualRgba& self, MjFloatRef4 joint) {
        MjFloat4(self.joint) = joint;
    }, py::return_value_policy::reference_internal);  mjVisualRgba.def_property(
      "actuator",
      [](raw::MjVisualRgba& self) -> MjFloat4 {
        return MjFloat4(self.actuator);
    },
      [](raw::MjVisualRgba& self, MjFloatRef4 actuator) {
        MjFloat4(self.actuator) = actuator;
    }, py::return_value_policy::reference_internal);  mjVisualRgba.def_property(
      "actuatornegative",
      [](raw::MjVisualRgba& self) -> MjFloat4 {
        return MjFloat4(self.actuatornegative);
    },
      [](raw::MjVisualRgba& self, MjFloatRef4 actuatornegative) {
        MjFloat4(self.actuatornegative) = actuatornegative;
    }, py::return_value_policy::reference_internal);  mjVisualRgba.def_property(
      "actuatorpositive",
      [](raw::MjVisualRgba& self) -> MjFloat4 {
        return MjFloat4(self.actuatorpositive);
    },
      [](raw::MjVisualRgba& self, MjFloatRef4 actuatorpositive) {
        MjFloat4(self.actuatorpositive) = actuatorpositive;
    }, py::return_value_policy::reference_internal);  mjVisualRgba.def_property(
      "com",
      [](raw::MjVisualRgba& self) -> MjFloat4 {
        return MjFloat4(self.com);
    },
      [](raw::MjVisualRgba& self, MjFloatRef4 com) {
        MjFloat4(self.com) = com;
    }, py::return_value_policy::reference_internal);  mjVisualRgba.def_property(
      "camera",
      [](raw::MjVisualRgba& self) -> MjFloat4 {
        return MjFloat4(self.camera);
    },
      [](raw::MjVisualRgba& self, MjFloatRef4 camera) {
        MjFloat4(self.camera) = camera;
    }, py::return_value_policy::reference_internal);  mjVisualRgba.def_property(
      "light",
      [](raw::MjVisualRgba& self) -> MjFloat4 {
        return MjFloat4(self.light);
    },
      [](raw::MjVisualRgba& self, MjFloatRef4 light) {
        MjFloat4(self.light) = light;
    }, py::return_value_policy::reference_internal);  mjVisualRgba.def_property(
      "selectpoint",
      [](raw::MjVisualRgba& self) -> MjFloat4 {
        return MjFloat4(self.selectpoint);
    },
      [](raw::MjVisualRgba& self, MjFloatRef4 selectpoint) {
        MjFloat4(self.selectpoint) = selectpoint;
    }, py::return_value_policy::reference_internal);  mjVisualRgba.def_property(
      "connect",
      [](raw::MjVisualRgba& self) -> MjFloat4 {
        return MjFloat4(self.connect);
    },
      [](raw::MjVisualRgba& self, MjFloatRef4 connect) {
        MjFloat4(self.connect) = connect;
    }, py::return_value_policy::reference_internal);  mjVisualRgba.def_property(
      "contactpoint",
      [](raw::MjVisualRgba& self) -> MjFloat4 {
        return MjFloat4(self.contactpoint);
    },
      [](raw::MjVisualRgba& self, MjFloatRef4 contactpoint) {
        MjFloat4(self.contactpoint) = contactpoint;
    }, py::return_value_policy::reference_internal);  mjVisualRgba.def_property(
      "contactforce",
      [](raw::MjVisualRgba& self) -> MjFloat4 {
        return MjFloat4(self.contactforce);
    },
      [](raw::MjVisualRgba& self, MjFloatRef4 contactforce) {
        MjFloat4(self.contactforce) = contactforce;
    }, py::return_value_policy::reference_internal);  mjVisualRgba.def_property(
      "contactfriction",
      [](raw::MjVisualRgba& self) -> MjFloat4 {
        return MjFloat4(self.contactfriction);
    },
      [](raw::MjVisualRgba& self, MjFloatRef4 contactfriction) {
        MjFloat4(self.contactfriction) = contactfriction;
    }, py::return_value_policy::reference_internal);  mjVisualRgba.def_property(
      "contacttorque",
      [](raw::MjVisualRgba& self) -> MjFloat4 {
        return MjFloat4(self.contacttorque);
    },
      [](raw::MjVisualRgba& self, MjFloatRef4 contacttorque) {
        MjFloat4(self.contacttorque) = contacttorque;
    }, py::return_value_policy::reference_internal);  mjVisualRgba.def_property(
      "contactgap",
      [](raw::MjVisualRgba& self) -> MjFloat4 {
        return MjFloat4(self.contactgap);
    },
      [](raw::MjVisualRgba& self, MjFloatRef4 contactgap) {
        MjFloat4(self.contactgap) = contactgap;
    }, py::return_value_policy::reference_internal);  mjVisualRgba.def_property(
      "rangefinder",
      [](raw::MjVisualRgba& self) -> MjFloat4 {
        return MjFloat4(self.rangefinder);
    },
      [](raw::MjVisualRgba& self, MjFloatRef4 rangefinder) {
        MjFloat4(self.rangefinder) = rangefinder;
    }, py::return_value_policy::reference_internal);  mjVisualRgba.def_property(
      "constraint",
      [](raw::MjVisualRgba& self) -> MjFloat4 {
        return MjFloat4(self.constraint);
    },
      [](raw::MjVisualRgba& self, MjFloatRef4 constraint) {
        MjFloat4(self.constraint) = constraint;
    }, py::return_value_policy::reference_internal);  mjVisualRgba.def_property(
      "slidercrank",
      [](raw::MjVisualRgba& self) -> MjFloat4 {
        return MjFloat4(self.slidercrank);
    },
      [](raw::MjVisualRgba& self, MjFloatRef4 slidercrank) {
        MjFloat4(self.slidercrank) = slidercrank;
    }, py::return_value_policy::reference_internal);  mjVisualRgba.def_property(
      "crankbroken",
      [](raw::MjVisualRgba& self) -> MjFloat4 {
        return MjFloat4(self.crankbroken);
    },
      [](raw::MjVisualRgba& self, MjFloatRef4 crankbroken) {
        MjFloat4(self.crankbroken) = crankbroken;
    }, py::return_value_policy::reference_internal);  mjVisualRgba.def_property(
      "frustum",
      [](raw::MjVisualRgba& self) -> MjFloat4 {
        return MjFloat4(self.frustum);
    },
      [](raw::MjVisualRgba& self, MjFloatRef4 frustum) {
        MjFloat4(self.frustum) = frustum;
    }, py::return_value_policy::reference_internal);  mjVisualRgba.def_property(
      "bv",
      [](raw::MjVisualRgba& self) -> MjFloat4 {
        return MjFloat4(self.bv);
    },
      [](raw::MjVisualRgba& self, MjFloatRef4 bv) {
        MjFloat4(self.bv) = bv;
    }, py::return_value_policy::reference_internal);  mjVisualRgba.def_property(
      "bvactive",
      [](raw::MjVisualRgba& self) -> MjFloat4 {
        return MjFloat4(self.bvactive);
    },
      [](raw::MjVisualRgba& self, MjFloatRef4 bvactive) {
        MjFloat4(self.bvactive) = bvactive;
    }, py::return_value_policy::reference_internal);mjVisual.def_property("rgba",[](raw::MjVisual& self) -> raw::MjVisualRgba& {
        return self.rgba;
      },[](raw::MjVisual& self, raw::MjVisualRgba& rgba) {
        self.rgba = rgba;
      },py::return_value_policy::reference_internal);

  // mjStatistic
mjStatistic.def_property("meaninertia",[](raw::MjStatistic& self) -> mjtNum {
        return self.meaninertia;
      },[](raw::MjStatistic& self, mjtNum meaninertia) {
        self.meaninertia = meaninertia;
      });
mjStatistic.def_property("meanmass",[](raw::MjStatistic& self) -> mjtNum {
        return self.meanmass;
      },[](raw::MjStatistic& self, mjtNum meanmass) {
        self.meanmass = meanmass;
      });
mjStatistic.def_property("meansize",[](raw::MjStatistic& self) -> mjtNum {
        return self.meansize;
      },[](raw::MjStatistic& self, mjtNum meansize) {
        self.meansize = meansize;
      });
mjStatistic.def_property("extent",[](raw::MjStatistic& self) -> mjtNum {
        return self.extent;
      },[](raw::MjStatistic& self, mjtNum extent) {
        self.extent = extent;
      });
  mjStatistic.def_property(
      "center",
      [](raw::MjStatistic& self) -> MjDouble3 {
        return MjDouble3(self.center);
    },
      [](raw::MjStatistic& self, MjDoubleRef3 center) {
        MjDouble3(self.center) = center;
    }, py::return_value_policy::reference_internal);

  // mjsCompiler
mjsCompiler.def_property("autolimits",[](raw::MjsCompiler& self) -> mjtByte {
        return self.autolimits;
      },[](raw::MjsCompiler& self, mjtByte autolimits) {
        self.autolimits = autolimits;
      });
mjsCompiler.def_property("boundmass",[](raw::MjsCompiler& self) -> double {
        return self.boundmass;
      },[](raw::MjsCompiler& self, double boundmass) {
        self.boundmass = boundmass;
      });
mjsCompiler.def_property("boundinertia",[](raw::MjsCompiler& self) -> double {
        return self.boundinertia;
      },[](raw::MjsCompiler& self, double boundinertia) {
        self.boundinertia = boundinertia;
      });
mjsCompiler.def_property("settotalmass",[](raw::MjsCompiler& self) -> double {
        return self.settotalmass;
      },[](raw::MjsCompiler& self, double settotalmass) {
        self.settotalmass = settotalmass;
      });
mjsCompiler.def_property("balanceinertia",[](raw::MjsCompiler& self) -> mjtByte {
        return self.balanceinertia;
      },[](raw::MjsCompiler& self, mjtByte balanceinertia) {
        self.balanceinertia = balanceinertia;
      });
mjsCompiler.def_property("fitaabb",[](raw::MjsCompiler& self) -> mjtByte {
        return self.fitaabb;
      },[](raw::MjsCompiler& self, mjtByte fitaabb) {
        self.fitaabb = fitaabb;
      });
mjsCompiler.def_property("degree",[](raw::MjsCompiler& self) -> mjtByte {
        return self.degree;
      },[](raw::MjsCompiler& self, mjtByte degree) {
        self.degree = degree;
      });
  mjsCompiler.def_property(
    "eulerseq",
    [](raw::MjsCompiler& self) -> MjTypeVec<char> {
      return MjTypeVec<char>(self.eulerseq, 3);
    },
    [](raw::MjsCompiler& self, py::object rhs) {
      int i = 0;
      for (auto val : rhs) {
        self.eulerseq[i++] = py::cast<char>(val);
      }
    }, py::return_value_policy::move);
mjsCompiler.def_property("discardvisual",[](raw::MjsCompiler& self) -> mjtByte {
        return self.discardvisual;
      },[](raw::MjsCompiler& self, mjtByte discardvisual) {
        self.discardvisual = discardvisual;
      });
mjsCompiler.def_property("usethread",[](raw::MjsCompiler& self) -> mjtByte {
        return self.usethread;
      },[](raw::MjsCompiler& self, mjtByte usethread) {
        self.usethread = usethread;
      });
mjsCompiler.def_property("fusestatic",[](raw::MjsCompiler& self) -> mjtByte {
        return self.fusestatic;
      },[](raw::MjsCompiler& self, mjtByte fusestatic) {
        self.fusestatic = fusestatic;
      });
mjsCompiler.def_property("inertiafromgeom",[](raw::MjsCompiler& self) -> int {
        return self.inertiafromgeom;
      },[](raw::MjsCompiler& self, int inertiafromgeom) {
        self.inertiafromgeom = inertiafromgeom;
      });
  mjsCompiler.def_property(
      "inertiagrouprange",
      [](raw::MjsCompiler& self) -> MjInt2 {
        return MjInt2(self.inertiagrouprange);
    },
      [](raw::MjsCompiler& self, MjIntRef2 inertiagrouprange) {
        MjInt2(self.inertiagrouprange) = inertiagrouprange;
    }, py::return_value_policy::reference_internal);
mjsCompiler.def_property("alignfree",[](raw::MjsCompiler& self) -> int {
        return self.alignfree;
      },[](raw::MjsCompiler& self, int alignfree) {
        self.alignfree = alignfree;
      });
mjsCompiler.def_property("LRopt",[](raw::MjsCompiler& self) -> mjLROpt& {
        return self.LRopt;
      },[](raw::MjsCompiler& self, mjLROpt& LRopt) {
        self.LRopt = LRopt;
      },py::return_value_policy::reference_internal);

  // mjSpec
  mjSpec.def_property(
      "modelname",
      [](MjSpec& self) -> std::string_view {
        return *self.ptr->modelname;
      },
      [](MjSpec& self, std::string_view modelname) {
        *(self.ptr->modelname) = modelname;
    });
mjSpec.def_property("compiler",[](MjSpec& self) -> raw::MjsCompiler& {
        return self.ptr->compiler;
      },[](MjSpec& self, raw::MjsCompiler& compiler) {
        self.ptr->compiler = compiler;
      },py::return_value_policy::reference_internal);
mjSpec.def_property("strippath",[](MjSpec& self) -> mjtByte {
        return self.ptr->strippath;
      },[](MjSpec& self, mjtByte strippath) {
        self.ptr->strippath = strippath;
      });
  mjSpec.def_property(
      "meshdir",
      [](MjSpec& self) -> std::string_view {
        return *self.ptr->meshdir;
      },
      [](MjSpec& self, std::string_view meshdir) {
        *(self.ptr->meshdir) = meshdir;
    });
  mjSpec.def_property(
      "texturedir",
      [](MjSpec& self) -> std::string_view {
        return *self.ptr->texturedir;
      },
      [](MjSpec& self, std::string_view texturedir) {
        *(self.ptr->texturedir) = texturedir;
    });
mjSpec.def_property("option",[](MjSpec& self) -> raw::MjOption& {
        return self.ptr->option;
      },[](MjSpec& self, raw::MjOption& option) {
        self.ptr->option = option;
      },py::return_value_policy::reference_internal);
mjSpec.def_property("visual",[](MjSpec& self) -> raw::MjVisual& {
        return self.ptr->visual;
      },[](MjSpec& self, raw::MjVisual& visual) {
        self.ptr->visual = visual;
      },py::return_value_policy::reference_internal);
mjSpec.def_property("stat",[](MjSpec& self) -> raw::MjStatistic& {
        return self.ptr->stat;
      },[](MjSpec& self, raw::MjStatistic& stat) {
        self.ptr->stat = stat;
      },py::return_value_policy::reference_internal);
mjSpec.def_property("memory",[](MjSpec& self) -> size_t& {
        return self.ptr->memory;
      },[](MjSpec& self, size_t& memory) {
        self.ptr->memory = memory;
      },py::return_value_policy::reference_internal);
mjSpec.def_property("nemax",[](MjSpec& self) -> int {
        return self.ptr->nemax;
      },[](MjSpec& self, int nemax) {
        self.ptr->nemax = nemax;
      });
mjSpec.def_property("nuserdata",[](MjSpec& self) -> int {
        return self.ptr->nuserdata;
      },[](MjSpec& self, int nuserdata) {
        self.ptr->nuserdata = nuserdata;
      });
mjSpec.def_property("nuser_body",[](MjSpec& self) -> int {
        return self.ptr->nuser_body;
      },[](MjSpec& self, int nuser_body) {
        self.ptr->nuser_body = nuser_body;
      });
mjSpec.def_property("nuser_jnt",[](MjSpec& self) -> int {
        return self.ptr->nuser_jnt;
      },[](MjSpec& self, int nuser_jnt) {
        self.ptr->nuser_jnt = nuser_jnt;
      });
mjSpec.def_property("nuser_geom",[](MjSpec& self) -> int {
        return self.ptr->nuser_geom;
      },[](MjSpec& self, int nuser_geom) {
        self.ptr->nuser_geom = nuser_geom;
      });
mjSpec.def_property("nuser_site",[](MjSpec& self) -> int {
        return self.ptr->nuser_site;
      },[](MjSpec& self, int nuser_site) {
        self.ptr->nuser_site = nuser_site;
      });
mjSpec.def_property("nuser_cam",[](MjSpec& self) -> int {
        return self.ptr->nuser_cam;
      },[](MjSpec& self, int nuser_cam) {
        self.ptr->nuser_cam = nuser_cam;
      });
mjSpec.def_property("nuser_tendon",[](MjSpec& self) -> int {
        return self.ptr->nuser_tendon;
      },[](MjSpec& self, int nuser_tendon) {
        self.ptr->nuser_tendon = nuser_tendon;
      });
mjSpec.def_property("nuser_actuator",[](MjSpec& self) -> int {
        return self.ptr->nuser_actuator;
      },[](MjSpec& self, int nuser_actuator) {
        self.ptr->nuser_actuator = nuser_actuator;
      });
mjSpec.def_property("nuser_sensor",[](MjSpec& self) -> int {
        return self.ptr->nuser_sensor;
      },[](MjSpec& self, int nuser_sensor) {
        self.ptr->nuser_sensor = nuser_sensor;
      });
mjSpec.def_property("nkey",[](MjSpec& self) -> int {
        return self.ptr->nkey;
      },[](MjSpec& self, int nkey) {
        self.ptr->nkey = nkey;
      });
mjSpec.def_property("njmax",[](MjSpec& self) -> int {
        return self.ptr->njmax;
      },[](MjSpec& self, int njmax) {
        self.ptr->njmax = njmax;
      });
mjSpec.def_property("nconmax",[](MjSpec& self) -> int {
        return self.ptr->nconmax;
      },[](MjSpec& self, int nconmax) {
        self.ptr->nconmax = nconmax;
      });
mjSpec.def_property("nstack",[](MjSpec& self) -> size_t& {
        return self.ptr->nstack;
      },[](MjSpec& self, size_t& nstack) {
        self.ptr->nstack = nstack;
      },py::return_value_policy::reference_internal);
  mjSpec.def_property(
      "comment",
      [](MjSpec& self) -> std::string_view {
        return *self.ptr->comment;
      },
      [](MjSpec& self, std::string_view comment) {
        *(self.ptr->comment) = comment;
    });
  mjSpec.def_property(
      "modelfiledir",
      [](MjSpec& self) -> std::string_view {
        return *self.ptr->modelfiledir;
      },
      [](MjSpec& self, std::string_view modelfiledir) {
        *(self.ptr->modelfiledir) = modelfiledir;
    });
mjSpec.def_property("hasImplicitPluginElem",[](MjSpec& self) -> mjtByte {
        return self.ptr->hasImplicitPluginElem;
      },[](MjSpec& self, mjtByte hasImplicitPluginElem) {
        self.ptr->hasImplicitPluginElem = hasImplicitPluginElem;
      });

  // mjsOrientation
mjsOrientation.def_property("type",[](raw::MjsOrientation& self) -> mjtOrientation& {
        return self.type;
      },[](raw::MjsOrientation& self, mjtOrientation& type) {
        self.type = type;
      },py::return_value_policy::reference_internal);
  mjsOrientation.def_property(
      "axisangle",
      [](raw::MjsOrientation& self) -> MjDouble4 {
        return MjDouble4(self.axisangle);
    },
      [](raw::MjsOrientation& self, MjDoubleRef4 axisangle) {
        MjDouble4(self.axisangle) = axisangle;
    }, py::return_value_policy::reference_internal);
  mjsOrientation.def_property(
      "xyaxes",
      [](raw::MjsOrientation& self) -> MjDouble6 {
        return MjDouble6(self.xyaxes);
    },
      [](raw::MjsOrientation& self, MjDoubleRef6 xyaxes) {
        MjDouble6(self.xyaxes) = xyaxes;
    }, py::return_value_policy::reference_internal);
  mjsOrientation.def_property(
      "zaxis",
      [](raw::MjsOrientation& self) -> MjDouble3 {
        return MjDouble3(self.zaxis);
    },
      [](raw::MjsOrientation& self, MjDoubleRef3 zaxis) {
        MjDouble3(self.zaxis) = zaxis;
    }, py::return_value_policy::reference_internal);
  mjsOrientation.def_property(
      "euler",
      [](raw::MjsOrientation& self) -> MjDouble3 {
        return MjDouble3(self.euler);
    },
      [](raw::MjsOrientation& self, MjDoubleRef3 euler) {
        MjDouble3(self.euler) = euler;
    }, py::return_value_policy::reference_internal);

  // mjsPlugin
  mjsPlugin.def_property(
      "name",
      [](raw::MjsPlugin& self) -> std::string_view {
        return *self.name;
      },
      [](raw::MjsPlugin& self, std::string_view name) {
        *(self.name) = name;
    });
  mjsPlugin.def_property(
      "plugin_name",
      [](raw::MjsPlugin& self) -> std::string_view {
        return *self.plugin_name;
      },
      [](raw::MjsPlugin& self, std::string_view plugin_name) {
        *(self.plugin_name) = plugin_name;
    });
mjsPlugin.def_property("active",[](raw::MjsPlugin& self) -> mjtByte {
        return self.active;
      },[](raw::MjsPlugin& self, mjtByte active) {
        self.active = active;
      });
  mjsPlugin.def_property(
      "info",
      [](raw::MjsPlugin& self) -> std::string_view {
        return *self.info;
      },
      [](raw::MjsPlugin& self, std::string_view info) {
        *(self.info) = info;
    });

  // mjsBody
  mjsBody.def_property(
      "name",
      [](raw::MjsBody& self) -> std::string_view {
        return *self.name;
      },
      [](raw::MjsBody& self, std::string_view name) {
        *(self.name) = name;
    });
  mjsBody.def_property(
      "childclass",
      [](raw::MjsBody& self) -> std::string_view {
        return *self.childclass;
      },
      [](raw::MjsBody& self, std::string_view childclass) {
        *(self.childclass) = childclass;
    });
  mjsBody.def_property(
      "pos",
      [](raw::MjsBody& self) -> MjDouble3 {
        return MjDouble3(self.pos);
    },
      [](raw::MjsBody& self, MjDoubleRef3 pos) {
        MjDouble3(self.pos) = pos;
    }, py::return_value_policy::reference_internal);
  mjsBody.def_property(
      "quat",
      [](raw::MjsBody& self) -> MjDouble4 {
        return MjDouble4(self.quat);
    },
      [](raw::MjsBody& self, MjDoubleRef4 quat) {
        MjDouble4(self.quat) = quat;
    }, py::return_value_policy::reference_internal);
mjsBody.def_property("alt",[](raw::MjsBody& self) -> raw::MjsOrientation& {
        return self.alt;
      },[](raw::MjsBody& self, raw::MjsOrientation& alt) {
        self.alt = alt;
      },py::return_value_policy::reference_internal);
mjsBody.def_property("mass",[](raw::MjsBody& self) -> double {
        return self.mass;
      },[](raw::MjsBody& self, double mass) {
        self.mass = mass;
      });
  mjsBody.def_property(
      "ipos",
      [](raw::MjsBody& self) -> MjDouble3 {
        return MjDouble3(self.ipos);
    },
      [](raw::MjsBody& self, MjDoubleRef3 ipos) {
        MjDouble3(self.ipos) = ipos;
    }, py::return_value_policy::reference_internal);
  mjsBody.def_property(
      "iquat",
      [](raw::MjsBody& self) -> MjDouble4 {
        return MjDouble4(self.iquat);
    },
      [](raw::MjsBody& self, MjDoubleRef4 iquat) {
        MjDouble4(self.iquat) = iquat;
    }, py::return_value_policy::reference_internal);
  mjsBody.def_property(
      "inertia",
      [](raw::MjsBody& self) -> MjDouble3 {
        return MjDouble3(self.inertia);
    },
      [](raw::MjsBody& self, MjDoubleRef3 inertia) {
        MjDouble3(self.inertia) = inertia;
    }, py::return_value_policy::reference_internal);
mjsBody.def_property("ialt",[](raw::MjsBody& self) -> raw::MjsOrientation& {
        return self.ialt;
      },[](raw::MjsBody& self, raw::MjsOrientation& ialt) {
        self.ialt = ialt;
      },py::return_value_policy::reference_internal);
  mjsBody.def_property(
      "fullinertia",
      [](raw::MjsBody& self) -> MjDouble6 {
        return MjDouble6(self.fullinertia);
    },
      [](raw::MjsBody& self, MjDoubleRef6 fullinertia) {
        MjDouble6(self.fullinertia) = fullinertia;
    }, py::return_value_policy::reference_internal);
mjsBody.def_property("mocap",[](raw::MjsBody& self) -> mjtByte {
        return self.mocap;
      },[](raw::MjsBody& self, mjtByte mocap) {
        self.mocap = mocap;
      });
mjsBody.def_property("gravcomp",[](raw::MjsBody& self) -> double {
        return self.gravcomp;
      },[](raw::MjsBody& self, double gravcomp) {
        self.gravcomp = gravcomp;
      });
  mjsBody.def_property(
    "userdata",
    [](raw::MjsBody& self) -> py::array_t<double> {
        return py::array_t<double>(self.userdata->size(),
                                      self.userdata->data());
      },
    [](raw::MjsBody& self, py::object rhs) {
        self.userdata->clear();
        self.userdata->reserve(py::len(rhs));
        for (auto val : rhs) {
          self.userdata->push_back(py::cast<double>(val));
      }
    }, py::return_value_policy::reference_internal);
mjsBody.def_property("explicitinertial",[](raw::MjsBody& self) -> mjtByte {
        return self.explicitinertial;
      },[](raw::MjsBody& self, mjtByte explicitinertial) {
        self.explicitinertial = explicitinertial;
      });
mjsBody.def_property("plugin",[](raw::MjsBody& self) -> raw::MjsPlugin& {
        return self.plugin;
      },[](raw::MjsBody& self, raw::MjsPlugin& plugin) {
        self.plugin = plugin;
      },py::return_value_policy::reference_internal);
  mjsBody.def_property(
      "info",
      [](raw::MjsBody& self) -> std::string_view {
        return *self.info;
      },
      [](raw::MjsBody& self, std::string_view info) {
        *(self.info) = info;
    });

  // mjsFrame
  mjsFrame.def_property(
      "name",
      [](raw::MjsFrame& self) -> std::string_view {
        return *self.name;
      },
      [](raw::MjsFrame& self, std::string_view name) {
        *(self.name) = name;
    });
  mjsFrame.def_property(
      "childclass",
      [](raw::MjsFrame& self) -> std::string_view {
        return *self.childclass;
      },
      [](raw::MjsFrame& self, std::string_view childclass) {
        *(self.childclass) = childclass;
    });
  mjsFrame.def_property(
      "pos",
      [](raw::MjsFrame& self) -> MjDouble3 {
        return MjDouble3(self.pos);
    },
      [](raw::MjsFrame& self, MjDoubleRef3 pos) {
        MjDouble3(self.pos) = pos;
    }, py::return_value_policy::reference_internal);
  mjsFrame.def_property(
      "quat",
      [](raw::MjsFrame& self) -> MjDouble4 {
        return MjDouble4(self.quat);
    },
      [](raw::MjsFrame& self, MjDoubleRef4 quat) {
        MjDouble4(self.quat) = quat;
    }, py::return_value_policy::reference_internal);
mjsFrame.def_property("alt",[](raw::MjsFrame& self) -> raw::MjsOrientation& {
        return self.alt;
      },[](raw::MjsFrame& self, raw::MjsOrientation& alt) {
        self.alt = alt;
      },py::return_value_policy::reference_internal);
  mjsFrame.def_property(
      "info",
      [](raw::MjsFrame& self) -> std::string_view {
        return *self.info;
      },
      [](raw::MjsFrame& self, std::string_view info) {
        *(self.info) = info;
    });

  // mjsJoint
  mjsJoint.def_property(
      "name",
      [](raw::MjsJoint& self) -> std::string_view {
        return *self.name;
      },
      [](raw::MjsJoint& self, std::string_view name) {
        *(self.name) = name;
    });
mjsJoint.def_property("type",[](raw::MjsJoint& self) -> mjtJoint& {
        return self.type;
      },[](raw::MjsJoint& self, mjtJoint& type) {
        self.type = type;
      },py::return_value_policy::reference_internal);
  mjsJoint.def_property(
      "pos",
      [](raw::MjsJoint& self) -> MjDouble3 {
        return MjDouble3(self.pos);
    },
      [](raw::MjsJoint& self, MjDoubleRef3 pos) {
        MjDouble3(self.pos) = pos;
    }, py::return_value_policy::reference_internal);
  mjsJoint.def_property(
      "axis",
      [](raw::MjsJoint& self) -> MjDouble3 {
        return MjDouble3(self.axis);
    },
      [](raw::MjsJoint& self, MjDoubleRef3 axis) {
        MjDouble3(self.axis) = axis;
    }, py::return_value_policy::reference_internal);
mjsJoint.def_property("ref",[](raw::MjsJoint& self) -> double {
        return self.ref;
      },[](raw::MjsJoint& self, double ref) {
        self.ref = ref;
      });
mjsJoint.def_property("align",[](raw::MjsJoint& self) -> int {
        return self.align;
      },[](raw::MjsJoint& self, int align) {
        self.align = align;
      });
mjsJoint.def_property("stiffness",[](raw::MjsJoint& self) -> double {
        return self.stiffness;
      },[](raw::MjsJoint& self, double stiffness) {
        self.stiffness = stiffness;
      });
mjsJoint.def_property("springref",[](raw::MjsJoint& self) -> double {
        return self.springref;
      },[](raw::MjsJoint& self, double springref) {
        self.springref = springref;
      });
  mjsJoint.def_property(
      "springdamper",
      [](raw::MjsJoint& self) -> MjDouble2 {
        return MjDouble2(self.springdamper);
    },
      [](raw::MjsJoint& self, MjDoubleRef2 springdamper) {
        MjDouble2(self.springdamper) = springdamper;
    }, py::return_value_policy::reference_internal);
mjsJoint.def_property("limited",[](raw::MjsJoint& self) -> int {
        return self.limited;
      },[](raw::MjsJoint& self, int limited) {
        self.limited = limited;
      });
  mjsJoint.def_property(
      "range",
      [](raw::MjsJoint& self) -> MjDouble2 {
        return MjDouble2(self.range);
    },
      [](raw::MjsJoint& self, MjDoubleRef2 range) {
        MjDouble2(self.range) = range;
    }, py::return_value_policy::reference_internal);
mjsJoint.def_property("margin",[](raw::MjsJoint& self) -> double {
        return self.margin;
      },[](raw::MjsJoint& self, double margin) {
        self.margin = margin;
      });
  mjsJoint.def_property(
      "solref_limit",
      [](raw::MjsJoint& self) -> MjDouble2 {
        return MjDouble2(self.solref_limit);
    },
      [](raw::MjsJoint& self, MjDoubleRef2 solref_limit) {
        MjDouble2(self.solref_limit) = solref_limit;
    }, py::return_value_policy::reference_internal);
  mjsJoint.def_property(
      "solimp_limit",
      [](raw::MjsJoint& self) -> MjDouble5 {
        return MjDouble5(self.solimp_limit);
    },
      [](raw::MjsJoint& self, MjDoubleRef5 solimp_limit) {
        MjDouble5(self.solimp_limit) = solimp_limit;
    }, py::return_value_policy::reference_internal);
mjsJoint.def_property("actfrclimited",[](raw::MjsJoint& self) -> int {
        return self.actfrclimited;
      },[](raw::MjsJoint& self, int actfrclimited) {
        self.actfrclimited = actfrclimited;
      });
  mjsJoint.def_property(
      "actfrcrange",
      [](raw::MjsJoint& self) -> MjDouble2 {
        return MjDouble2(self.actfrcrange);
    },
      [](raw::MjsJoint& self, MjDoubleRef2 actfrcrange) {
        MjDouble2(self.actfrcrange) = actfrcrange;
    }, py::return_value_policy::reference_internal);
mjsJoint.def_property("armature",[](raw::MjsJoint& self) -> double {
        return self.armature;
      },[](raw::MjsJoint& self, double armature) {
        self.armature = armature;
      });
mjsJoint.def_property("damping",[](raw::MjsJoint& self) -> double {
        return self.damping;
      },[](raw::MjsJoint& self, double damping) {
        self.damping = damping;
      });
mjsJoint.def_property("frictionloss",[](raw::MjsJoint& self) -> double {
        return self.frictionloss;
      },[](raw::MjsJoint& self, double frictionloss) {
        self.frictionloss = frictionloss;
      });
  mjsJoint.def_property(
      "solref_friction",
      [](raw::MjsJoint& self) -> MjDouble2 {
        return MjDouble2(self.solref_friction);
    },
      [](raw::MjsJoint& self, MjDoubleRef2 solref_friction) {
        MjDouble2(self.solref_friction) = solref_friction;
    }, py::return_value_policy::reference_internal);
  mjsJoint.def_property(
      "solimp_friction",
      [](raw::MjsJoint& self) -> MjDouble5 {
        return MjDouble5(self.solimp_friction);
    },
      [](raw::MjsJoint& self, MjDoubleRef5 solimp_friction) {
        MjDouble5(self.solimp_friction) = solimp_friction;
    }, py::return_value_policy::reference_internal);
mjsJoint.def_property("group",[](raw::MjsJoint& self) -> int {
        return self.group;
      },[](raw::MjsJoint& self, int group) {
        self.group = group;
      });
mjsJoint.def_property("actgravcomp",[](raw::MjsJoint& self) -> mjtByte {
        return self.actgravcomp;
      },[](raw::MjsJoint& self, mjtByte actgravcomp) {
        self.actgravcomp = actgravcomp;
      });
  mjsJoint.def_property(
    "userdata",
    [](raw::MjsJoint& self) -> py::array_t<double> {
        return py::array_t<double>(self.userdata->size(),
                                      self.userdata->data());
      },
    [](raw::MjsJoint& self, py::object rhs) {
        self.userdata->clear();
        self.userdata->reserve(py::len(rhs));
        for (auto val : rhs) {
          self.userdata->push_back(py::cast<double>(val));
      }
    }, py::return_value_policy::reference_internal);
  mjsJoint.def_property(
      "info",
      [](raw::MjsJoint& self) -> std::string_view {
        return *self.info;
      },
      [](raw::MjsJoint& self, std::string_view info) {
        *(self.info) = info;
    });

  // mjsGeom
  mjsGeom.def_property(
      "name",
      [](raw::MjsGeom& self) -> std::string_view {
        return *self.name;
      },
      [](raw::MjsGeom& self, std::string_view name) {
        *(self.name) = name;
    });
mjsGeom.def_property("type",[](raw::MjsGeom& self) -> mjtGeom& {
        return self.type;
      },[](raw::MjsGeom& self, mjtGeom& type) {
        self.type = type;
      },py::return_value_policy::reference_internal);
  mjsGeom.def_property(
      "pos",
      [](raw::MjsGeom& self) -> MjDouble3 {
        return MjDouble3(self.pos);
    },
      [](raw::MjsGeom& self, MjDoubleRef3 pos) {
        MjDouble3(self.pos) = pos;
    }, py::return_value_policy::reference_internal);
  mjsGeom.def_property(
      "quat",
      [](raw::MjsGeom& self) -> MjDouble4 {
        return MjDouble4(self.quat);
    },
      [](raw::MjsGeom& self, MjDoubleRef4 quat) {
        MjDouble4(self.quat) = quat;
    }, py::return_value_policy::reference_internal);
mjsGeom.def_property("alt",[](raw::MjsGeom& self) -> raw::MjsOrientation& {
        return self.alt;
      },[](raw::MjsGeom& self, raw::MjsOrientation& alt) {
        self.alt = alt;
      },py::return_value_policy::reference_internal);
  mjsGeom.def_property(
      "fromto",
      [](raw::MjsGeom& self) -> MjDouble6 {
        return MjDouble6(self.fromto);
    },
      [](raw::MjsGeom& self, MjDoubleRef6 fromto) {
        MjDouble6(self.fromto) = fromto;
    }, py::return_value_policy::reference_internal);
  mjsGeom.def_property(
      "size",
      [](raw::MjsGeom& self) -> MjDouble3 {
        return MjDouble3(self.size);
    },
      [](raw::MjsGeom& self, MjDoubleRef3 size) {
        MjDouble3(self.size) = size;
    }, py::return_value_policy::reference_internal);
mjsGeom.def_property("contype",[](raw::MjsGeom& self) -> int {
        return self.contype;
      },[](raw::MjsGeom& self, int contype) {
        self.contype = contype;
      });
mjsGeom.def_property("conaffinity",[](raw::MjsGeom& self) -> int {
        return self.conaffinity;
      },[](raw::MjsGeom& self, int conaffinity) {
        self.conaffinity = conaffinity;
      });
mjsGeom.def_property("condim",[](raw::MjsGeom& self) -> int {
        return self.condim;
      },[](raw::MjsGeom& self, int condim) {
        self.condim = condim;
      });
mjsGeom.def_property("priority",[](raw::MjsGeom& self) -> int {
        return self.priority;
      },[](raw::MjsGeom& self, int priority) {
        self.priority = priority;
      });
  mjsGeom.def_property(
      "friction",
      [](raw::MjsGeom& self) -> MjDouble3 {
        return MjDouble3(self.friction);
    },
      [](raw::MjsGeom& self, MjDoubleRef3 friction) {
        MjDouble3(self.friction) = friction;
    }, py::return_value_policy::reference_internal);
mjsGeom.def_property("solmix",[](raw::MjsGeom& self) -> double {
        return self.solmix;
      },[](raw::MjsGeom& self, double solmix) {
        self.solmix = solmix;
      });
  mjsGeom.def_property(
      "solref",
      [](raw::MjsGeom& self) -> MjDouble2 {
        return MjDouble2(self.solref);
    },
      [](raw::MjsGeom& self, MjDoubleRef2 solref) {
        MjDouble2(self.solref) = solref;
    }, py::return_value_policy::reference_internal);
  mjsGeom.def_property(
      "solimp",
      [](raw::MjsGeom& self) -> MjDouble5 {
        return MjDouble5(self.solimp);
    },
      [](raw::MjsGeom& self, MjDoubleRef5 solimp) {
        MjDouble5(self.solimp) = solimp;
    }, py::return_value_policy::reference_internal);
mjsGeom.def_property("margin",[](raw::MjsGeom& self) -> double {
        return self.margin;
      },[](raw::MjsGeom& self, double margin) {
        self.margin = margin;
      });
mjsGeom.def_property("gap",[](raw::MjsGeom& self) -> double {
        return self.gap;
      },[](raw::MjsGeom& self, double gap) {
        self.gap = gap;
      });
mjsGeom.def_property("mass",[](raw::MjsGeom& self) -> double {
        return self.mass;
      },[](raw::MjsGeom& self, double mass) {
        self.mass = mass;
      });
mjsGeom.def_property("density",[](raw::MjsGeom& self) -> double {
        return self.density;
      },[](raw::MjsGeom& self, double density) {
        self.density = density;
      });
mjsGeom.def_property("typeinertia",[](raw::MjsGeom& self) -> mjtGeomInertia& {
        return self.typeinertia;
      },[](raw::MjsGeom& self, mjtGeomInertia& typeinertia) {
        self.typeinertia = typeinertia;
      },py::return_value_policy::reference_internal);
mjsGeom.def_property("fluid_ellipsoid",[](raw::MjsGeom& self) -> mjtNum {
        return self.fluid_ellipsoid;
      },[](raw::MjsGeom& self, mjtNum fluid_ellipsoid) {
        self.fluid_ellipsoid = fluid_ellipsoid;
      });
  mjsGeom.def_property(
      "fluid_coefs",
      [](raw::MjsGeom& self) -> MjDouble5 {
        return MjDouble5(self.fluid_coefs);
    },
      [](raw::MjsGeom& self, MjDoubleRef5 fluid_coefs) {
        MjDouble5(self.fluid_coefs) = fluid_coefs;
    }, py::return_value_policy::reference_internal);
  mjsGeom.def_property(
      "material",
      [](raw::MjsGeom& self) -> std::string_view {
        return *self.material;
      },
      [](raw::MjsGeom& self, std::string_view material) {
        *(self.material) = material;
    });
  mjsGeom.def_property(
      "rgba",
      [](raw::MjsGeom& self) -> MjFloat4 {
        return MjFloat4(self.rgba);
    },
      [](raw::MjsGeom& self, MjFloatRef4 rgba) {
        MjFloat4(self.rgba) = rgba;
    }, py::return_value_policy::reference_internal);
mjsGeom.def_property("group",[](raw::MjsGeom& self) -> int {
        return self.group;
      },[](raw::MjsGeom& self, int group) {
        self.group = group;
      });
  mjsGeom.def_property(
      "hfieldname",
      [](raw::MjsGeom& self) -> std::string_view {
        return *self.hfieldname;
      },
      [](raw::MjsGeom& self, std::string_view hfieldname) {
        *(self.hfieldname) = hfieldname;
    });
  mjsGeom.def_property(
      "meshname",
      [](raw::MjsGeom& self) -> std::string_view {
        return *self.meshname;
      },
      [](raw::MjsGeom& self, std::string_view meshname) {
        *(self.meshname) = meshname;
    });
mjsGeom.def_property("fitscale",[](raw::MjsGeom& self) -> double {
        return self.fitscale;
      },[](raw::MjsGeom& self, double fitscale) {
        self.fitscale = fitscale;
      });
  mjsGeom.def_property(
    "userdata",
    [](raw::MjsGeom& self) -> py::array_t<double> {
        return py::array_t<double>(self.userdata->size(),
                                      self.userdata->data());
      },
    [](raw::MjsGeom& self, py::object rhs) {
        self.userdata->clear();
        self.userdata->reserve(py::len(rhs));
        for (auto val : rhs) {
          self.userdata->push_back(py::cast<double>(val));
      }
    }, py::return_value_policy::reference_internal);
mjsGeom.def_property("plugin",[](raw::MjsGeom& self) -> raw::MjsPlugin& {
        return self.plugin;
      },[](raw::MjsGeom& self, raw::MjsPlugin& plugin) {
        self.plugin = plugin;
      },py::return_value_policy::reference_internal);
  mjsGeom.def_property(
      "info",
      [](raw::MjsGeom& self) -> std::string_view {
        return *self.info;
      },
      [](raw::MjsGeom& self, std::string_view info) {
        *(self.info) = info;
    });

  // mjsSite
  mjsSite.def_property(
      "name",
      [](raw::MjsSite& self) -> std::string_view {
        return *self.name;
      },
      [](raw::MjsSite& self, std::string_view name) {
        *(self.name) = name;
    });
  mjsSite.def_property(
      "pos",
      [](raw::MjsSite& self) -> MjDouble3 {
        return MjDouble3(self.pos);
    },
      [](raw::MjsSite& self, MjDoubleRef3 pos) {
        MjDouble3(self.pos) = pos;
    }, py::return_value_policy::reference_internal);
  mjsSite.def_property(
      "quat",
      [](raw::MjsSite& self) -> MjDouble4 {
        return MjDouble4(self.quat);
    },
      [](raw::MjsSite& self, MjDoubleRef4 quat) {
        MjDouble4(self.quat) = quat;
    }, py::return_value_policy::reference_internal);
mjsSite.def_property("alt",[](raw::MjsSite& self) -> raw::MjsOrientation& {
        return self.alt;
      },[](raw::MjsSite& self, raw::MjsOrientation& alt) {
        self.alt = alt;
      },py::return_value_policy::reference_internal);
  mjsSite.def_property(
      "fromto",
      [](raw::MjsSite& self) -> MjDouble6 {
        return MjDouble6(self.fromto);
    },
      [](raw::MjsSite& self, MjDoubleRef6 fromto) {
        MjDouble6(self.fromto) = fromto;
    }, py::return_value_policy::reference_internal);
  mjsSite.def_property(
      "size",
      [](raw::MjsSite& self) -> MjDouble3 {
        return MjDouble3(self.size);
    },
      [](raw::MjsSite& self, MjDoubleRef3 size) {
        MjDouble3(self.size) = size;
    }, py::return_value_policy::reference_internal);
mjsSite.def_property("type",[](raw::MjsSite& self) -> mjtGeom& {
        return self.type;
      },[](raw::MjsSite& self, mjtGeom& type) {
        self.type = type;
      },py::return_value_policy::reference_internal);
  mjsSite.def_property(
      "material",
      [](raw::MjsSite& self) -> std::string_view {
        return *self.material;
      },
      [](raw::MjsSite& self, std::string_view material) {
        *(self.material) = material;
    });
mjsSite.def_property("group",[](raw::MjsSite& self) -> int {
        return self.group;
      },[](raw::MjsSite& self, int group) {
        self.group = group;
      });
  mjsSite.def_property(
      "rgba",
      [](raw::MjsSite& self) -> MjFloat4 {
        return MjFloat4(self.rgba);
    },
      [](raw::MjsSite& self, MjFloatRef4 rgba) {
        MjFloat4(self.rgba) = rgba;
    }, py::return_value_policy::reference_internal);
  mjsSite.def_property(
    "userdata",
    [](raw::MjsSite& self) -> py::array_t<double> {
        return py::array_t<double>(self.userdata->size(),
                                      self.userdata->data());
      },
    [](raw::MjsSite& self, py::object rhs) {
        self.userdata->clear();
        self.userdata->reserve(py::len(rhs));
        for (auto val : rhs) {
          self.userdata->push_back(py::cast<double>(val));
      }
    }, py::return_value_policy::reference_internal);
  mjsSite.def_property(
      "info",
      [](raw::MjsSite& self) -> std::string_view {
        return *self.info;
      },
      [](raw::MjsSite& self, std::string_view info) {
        *(self.info) = info;
    });

  // mjsCamera
  mjsCamera.def_property(
      "name",
      [](raw::MjsCamera& self) -> std::string_view {
        return *self.name;
      },
      [](raw::MjsCamera& self, std::string_view name) {
        *(self.name) = name;
    });
  mjsCamera.def_property(
      "pos",
      [](raw::MjsCamera& self) -> MjDouble3 {
        return MjDouble3(self.pos);
    },
      [](raw::MjsCamera& self, MjDoubleRef3 pos) {
        MjDouble3(self.pos) = pos;
    }, py::return_value_policy::reference_internal);
  mjsCamera.def_property(
      "quat",
      [](raw::MjsCamera& self) -> MjDouble4 {
        return MjDouble4(self.quat);
    },
      [](raw::MjsCamera& self, MjDoubleRef4 quat) {
        MjDouble4(self.quat) = quat;
    }, py::return_value_policy::reference_internal);
mjsCamera.def_property("alt",[](raw::MjsCamera& self) -> raw::MjsOrientation& {
        return self.alt;
      },[](raw::MjsCamera& self, raw::MjsOrientation& alt) {
        self.alt = alt;
      },py::return_value_policy::reference_internal);
mjsCamera.def_property("mode",[](raw::MjsCamera& self) -> mjtCamLight& {
        return self.mode;
      },[](raw::MjsCamera& self, mjtCamLight& mode) {
        self.mode = mode;
      },py::return_value_policy::reference_internal);
  mjsCamera.def_property(
      "targetbody",
      [](raw::MjsCamera& self) -> std::string_view {
        return *self.targetbody;
      },
      [](raw::MjsCamera& self, std::string_view targetbody) {
        *(self.targetbody) = targetbody;
    });
mjsCamera.def_property("orthographic",[](raw::MjsCamera& self) -> int {
        return self.orthographic;
      },[](raw::MjsCamera& self, int orthographic) {
        self.orthographic = orthographic;
      });
mjsCamera.def_property("fovy",[](raw::MjsCamera& self) -> double {
        return self.fovy;
      },[](raw::MjsCamera& self, double fovy) {
        self.fovy = fovy;
      });
mjsCamera.def_property("ipd",[](raw::MjsCamera& self) -> double {
        return self.ipd;
      },[](raw::MjsCamera& self, double ipd) {
        self.ipd = ipd;
      });
  mjsCamera.def_property(
      "intrinsic",
      [](raw::MjsCamera& self) -> MjFloat4 {
        return MjFloat4(self.intrinsic);
    },
      [](raw::MjsCamera& self, MjFloatRef4 intrinsic) {
        MjFloat4(self.intrinsic) = intrinsic;
    }, py::return_value_policy::reference_internal);
  mjsCamera.def_property(
      "sensor_size",
      [](raw::MjsCamera& self) -> MjFloat2 {
        return MjFloat2(self.sensor_size);
    },
      [](raw::MjsCamera& self, MjFloatRef2 sensor_size) {
        MjFloat2(self.sensor_size) = sensor_size;
    }, py::return_value_policy::reference_internal);
  mjsCamera.def_property(
      "resolution",
      [](raw::MjsCamera& self) -> MjFloat2 {
        return MjFloat2(self.resolution);
    },
      [](raw::MjsCamera& self, MjFloatRef2 resolution) {
        MjFloat2(self.resolution) = resolution;
    }, py::return_value_policy::reference_internal);
  mjsCamera.def_property(
      "focal_length",
      [](raw::MjsCamera& self) -> MjFloat2 {
        return MjFloat2(self.focal_length);
    },
      [](raw::MjsCamera& self, MjFloatRef2 focal_length) {
        MjFloat2(self.focal_length) = focal_length;
    }, py::return_value_policy::reference_internal);
  mjsCamera.def_property(
      "focal_pixel",
      [](raw::MjsCamera& self) -> MjFloat2 {
        return MjFloat2(self.focal_pixel);
    },
      [](raw::MjsCamera& self, MjFloatRef2 focal_pixel) {
        MjFloat2(self.focal_pixel) = focal_pixel;
    }, py::return_value_policy::reference_internal);
  mjsCamera.def_property(
      "principal_length",
      [](raw::MjsCamera& self) -> MjFloat2 {
        return MjFloat2(self.principal_length);
    },
      [](raw::MjsCamera& self, MjFloatRef2 principal_length) {
        MjFloat2(self.principal_length) = principal_length;
    }, py::return_value_policy::reference_internal);
  mjsCamera.def_property(
      "principal_pixel",
      [](raw::MjsCamera& self) -> MjFloat2 {
        return MjFloat2(self.principal_pixel);
    },
      [](raw::MjsCamera& self, MjFloatRef2 principal_pixel) {
        MjFloat2(self.principal_pixel) = principal_pixel;
    }, py::return_value_policy::reference_internal);
  mjsCamera.def_property(
    "userdata",
    [](raw::MjsCamera& self) -> py::array_t<double> {
        return py::array_t<double>(self.userdata->size(),
                                      self.userdata->data());
      },
    [](raw::MjsCamera& self, py::object rhs) {
        self.userdata->clear();
        self.userdata->reserve(py::len(rhs));
        for (auto val : rhs) {
          self.userdata->push_back(py::cast<double>(val));
      }
    }, py::return_value_policy::reference_internal);
  mjsCamera.def_property(
      "info",
      [](raw::MjsCamera& self) -> std::string_view {
        return *self.info;
      },
      [](raw::MjsCamera& self, std::string_view info) {
        *(self.info) = info;
    });

  // mjsLight
  mjsLight.def_property(
      "name",
      [](raw::MjsLight& self) -> std::string_view {
        return *self.name;
      },
      [](raw::MjsLight& self, std::string_view name) {
        *(self.name) = name;
    });
  mjsLight.def_property(
      "pos",
      [](raw::MjsLight& self) -> MjDouble3 {
        return MjDouble3(self.pos);
    },
      [](raw::MjsLight& self, MjDoubleRef3 pos) {
        MjDouble3(self.pos) = pos;
    }, py::return_value_policy::reference_internal);
  mjsLight.def_property(
      "dir",
      [](raw::MjsLight& self) -> MjDouble3 {
        return MjDouble3(self.dir);
    },
      [](raw::MjsLight& self, MjDoubleRef3 dir) {
        MjDouble3(self.dir) = dir;
    }, py::return_value_policy::reference_internal);
mjsLight.def_property("mode",[](raw::MjsLight& self) -> mjtCamLight& {
        return self.mode;
      },[](raw::MjsLight& self, mjtCamLight& mode) {
        self.mode = mode;
      },py::return_value_policy::reference_internal);
  mjsLight.def_property(
      "targetbody",
      [](raw::MjsLight& self) -> std::string_view {
        return *self.targetbody;
      },
      [](raw::MjsLight& self, std::string_view targetbody) {
        *(self.targetbody) = targetbody;
    });
mjsLight.def_property("active",[](raw::MjsLight& self) -> mjtByte {
        return self.active;
      },[](raw::MjsLight& self, mjtByte active) {
        self.active = active;
      });
mjsLight.def_property("directional",[](raw::MjsLight& self) -> mjtByte {
        return self.directional;
      },[](raw::MjsLight& self, mjtByte directional) {
        self.directional = directional;
      });
mjsLight.def_property("castshadow",[](raw::MjsLight& self) -> mjtByte {
        return self.castshadow;
      },[](raw::MjsLight& self, mjtByte castshadow) {
        self.castshadow = castshadow;
      });
mjsLight.def_property("bulbradius",[](raw::MjsLight& self) -> double {
        return self.bulbradius;
      },[](raw::MjsLight& self, double bulbradius) {
        self.bulbradius = bulbradius;
      });
  mjsLight.def_property(
      "attenuation",
      [](raw::MjsLight& self) -> MjFloat3 {
        return MjFloat3(self.attenuation);
    },
      [](raw::MjsLight& self, MjFloatRef3 attenuation) {
        MjFloat3(self.attenuation) = attenuation;
    }, py::return_value_policy::reference_internal);
mjsLight.def_property("cutoff",[](raw::MjsLight& self) -> float {
        return self.cutoff;
      },[](raw::MjsLight& self, float cutoff) {
        self.cutoff = cutoff;
      });
mjsLight.def_property("exponent",[](raw::MjsLight& self) -> float {
        return self.exponent;
      },[](raw::MjsLight& self, float exponent) {
        self.exponent = exponent;
      });
  mjsLight.def_property(
      "ambient",
      [](raw::MjsLight& self) -> MjFloat3 {
        return MjFloat3(self.ambient);
    },
      [](raw::MjsLight& self, MjFloatRef3 ambient) {
        MjFloat3(self.ambient) = ambient;
    }, py::return_value_policy::reference_internal);
  mjsLight.def_property(
      "diffuse",
      [](raw::MjsLight& self) -> MjFloat3 {
        return MjFloat3(self.diffuse);
    },
      [](raw::MjsLight& self, MjFloatRef3 diffuse) {
        MjFloat3(self.diffuse) = diffuse;
    }, py::return_value_policy::reference_internal);
  mjsLight.def_property(
      "specular",
      [](raw::MjsLight& self) -> MjFloat3 {
        return MjFloat3(self.specular);
    },
      [](raw::MjsLight& self, MjFloatRef3 specular) {
        MjFloat3(self.specular) = specular;
    }, py::return_value_policy::reference_internal);
  mjsLight.def_property(
      "info",
      [](raw::MjsLight& self) -> std::string_view {
        return *self.info;
      },
      [](raw::MjsLight& self, std::string_view info) {
        *(self.info) = info;
    });

  // mjsFlex
  mjsFlex.def_property(
      "name",
      [](raw::MjsFlex& self) -> std::string_view {
        return *self.name;
      },
      [](raw::MjsFlex& self, std::string_view name) {
        *(self.name) = name;
    });
mjsFlex.def_property("contype",[](raw::MjsFlex& self) -> int {
        return self.contype;
      },[](raw::MjsFlex& self, int contype) {
        self.contype = contype;
      });
mjsFlex.def_property("conaffinity",[](raw::MjsFlex& self) -> int {
        return self.conaffinity;
      },[](raw::MjsFlex& self, int conaffinity) {
        self.conaffinity = conaffinity;
      });
mjsFlex.def_property("condim",[](raw::MjsFlex& self) -> int {
        return self.condim;
      },[](raw::MjsFlex& self, int condim) {
        self.condim = condim;
      });
mjsFlex.def_property("priority",[](raw::MjsFlex& self) -> int {
        return self.priority;
      },[](raw::MjsFlex& self, int priority) {
        self.priority = priority;
      });
  mjsFlex.def_property(
      "friction",
      [](raw::MjsFlex& self) -> MjDouble3 {
        return MjDouble3(self.friction);
    },
      [](raw::MjsFlex& self, MjDoubleRef3 friction) {
        MjDouble3(self.friction) = friction;
    }, py::return_value_policy::reference_internal);
mjsFlex.def_property("solmix",[](raw::MjsFlex& self) -> double {
        return self.solmix;
      },[](raw::MjsFlex& self, double solmix) {
        self.solmix = solmix;
      });
  mjsFlex.def_property(
      "solref",
      [](raw::MjsFlex& self) -> MjDouble2 {
        return MjDouble2(self.solref);
    },
      [](raw::MjsFlex& self, MjDoubleRef2 solref) {
        MjDouble2(self.solref) = solref;
    }, py::return_value_policy::reference_internal);
  mjsFlex.def_property(
      "solimp",
      [](raw::MjsFlex& self) -> MjDouble5 {
        return MjDouble5(self.solimp);
    },
      [](raw::MjsFlex& self, MjDoubleRef5 solimp) {
        MjDouble5(self.solimp) = solimp;
    }, py::return_value_policy::reference_internal);
mjsFlex.def_property("margin",[](raw::MjsFlex& self) -> double {
        return self.margin;
      },[](raw::MjsFlex& self, double margin) {
        self.margin = margin;
      });
mjsFlex.def_property("gap",[](raw::MjsFlex& self) -> double {
        return self.gap;
      },[](raw::MjsFlex& self, double gap) {
        self.gap = gap;
      });
mjsFlex.def_property("dim",[](raw::MjsFlex& self) -> int {
        return self.dim;
      },[](raw::MjsFlex& self, int dim) {
        self.dim = dim;
      });
mjsFlex.def_property("radius",[](raw::MjsFlex& self) -> double {
        return self.radius;
      },[](raw::MjsFlex& self, double radius) {
        self.radius = radius;
      });
mjsFlex.def_property("internal",[](raw::MjsFlex& self) -> mjtByte {
        return self.internal;
      },[](raw::MjsFlex& self, mjtByte internal) {
        self.internal = internal;
      });
mjsFlex.def_property("flatskin",[](raw::MjsFlex& self) -> mjtByte {
        return self.flatskin;
      },[](raw::MjsFlex& self, mjtByte flatskin) {
        self.flatskin = flatskin;
      });
mjsFlex.def_property("selfcollide",[](raw::MjsFlex& self) -> int {
        return self.selfcollide;
      },[](raw::MjsFlex& self, int selfcollide) {
        self.selfcollide = selfcollide;
      });
mjsFlex.def_property("activelayers",[](raw::MjsFlex& self) -> int {
        return self.activelayers;
      },[](raw::MjsFlex& self, int activelayers) {
        self.activelayers = activelayers;
      });
mjsFlex.def_property("group",[](raw::MjsFlex& self) -> int {
        return self.group;
      },[](raw::MjsFlex& self, int group) {
        self.group = group;
      });
mjsFlex.def_property("edgestiffness",[](raw::MjsFlex& self) -> double {
        return self.edgestiffness;
      },[](raw::MjsFlex& self, double edgestiffness) {
        self.edgestiffness = edgestiffness;
      });
mjsFlex.def_property("edgedamping",[](raw::MjsFlex& self) -> double {
        return self.edgedamping;
      },[](raw::MjsFlex& self, double edgedamping) {
        self.edgedamping = edgedamping;
      });
  mjsFlex.def_property(
      "rgba",
      [](raw::MjsFlex& self) -> MjFloat4 {
        return MjFloat4(self.rgba);
    },
      [](raw::MjsFlex& self, MjFloatRef4 rgba) {
        MjFloat4(self.rgba) = rgba;
    }, py::return_value_policy::reference_internal);
  mjsFlex.def_property(
      "material",
      [](raw::MjsFlex& self) -> std::string_view {
        return *self.material;
      },
      [](raw::MjsFlex& self, std::string_view material) {
        *(self.material) = material;
    });
mjsFlex.def_property("young",[](raw::MjsFlex& self) -> double {
        return self.young;
      },[](raw::MjsFlex& self, double young) {
        self.young = young;
      });
mjsFlex.def_property("poisson",[](raw::MjsFlex& self) -> double {
        return self.poisson;
      },[](raw::MjsFlex& self, double poisson) {
        self.poisson = poisson;
      });
mjsFlex.def_property("damping",[](raw::MjsFlex& self) -> double {
        return self.damping;
      },[](raw::MjsFlex& self, double damping) {
        self.damping = damping;
      });
mjsFlex.def_property("thickness",[](raw::MjsFlex& self) -> double {
        return self.thickness;
      },[](raw::MjsFlex& self, double thickness) {
        self.thickness = thickness;
      });
  mjsFlex.def_property(
    "nodebody",
    [](raw::MjsFlex& self) -> MjTypeVec<std::string> {
        return MjTypeVec<std::string>(self.nodebody->data(),
                                      self.nodebody->size());
      },
    [](raw::MjsFlex& self, py::object rhs) {
        self.nodebody->clear();
        self.nodebody->reserve(py::len(rhs));
        for (auto val : rhs) {
          self.nodebody->push_back(py::cast<std::string>(val));
      }
    }, py::return_value_policy::move);
  mjsFlex.def_property(
    "vertbody",
    [](raw::MjsFlex& self) -> MjTypeVec<std::string> {
        return MjTypeVec<std::string>(self.vertbody->data(),
                                      self.vertbody->size());
      },
    [](raw::MjsFlex& self, py::object rhs) {
        self.vertbody->clear();
        self.vertbody->reserve(py::len(rhs));
        for (auto val : rhs) {
          self.vertbody->push_back(py::cast<std::string>(val));
      }
    }, py::return_value_policy::move);
  mjsFlex.def_property(
    "node",
    [](raw::MjsFlex& self) -> py::array_t<double> {
        return py::array_t<double>(self.node->size(),
                                      self.node->data());
      },
    [](raw::MjsFlex& self, py::object rhs) {
        self.node->clear();
        self.node->reserve(py::len(rhs));
        for (auto val : rhs) {
          self.node->push_back(py::cast<double>(val));
      }
    }, py::return_value_policy::reference_internal);
  mjsFlex.def_property(
    "vert",
    [](raw::MjsFlex& self) -> py::array_t<double> {
        return py::array_t<double>(self.vert->size(),
                                      self.vert->data());
      },
    [](raw::MjsFlex& self, py::object rhs) {
        self.vert->clear();
        self.vert->reserve(py::len(rhs));
        for (auto val : rhs) {
          self.vert->push_back(py::cast<double>(val));
      }
    }, py::return_value_policy::reference_internal);
  mjsFlex.def_property(
    "elem",
    [](raw::MjsFlex& self) -> py::array_t<int> {
        return py::array_t<int>(self.elem->size(),
                                      self.elem->data());
      },
    [](raw::MjsFlex& self, py::object rhs) {
        self.elem->clear();
        self.elem->reserve(py::len(rhs));
        for (auto val : rhs) {
          self.elem->push_back(py::cast<int>(val));
      }
    }, py::return_value_policy::reference_internal);
  mjsFlex.def_property(
    "texcoord",
    [](raw::MjsFlex& self) -> py::array_t<float> {
        return py::array_t<float>(self.texcoord->size(),
                                      self.texcoord->data());
      },
    [](raw::MjsFlex& self, py::object rhs) {
        self.texcoord->clear();
        self.texcoord->reserve(py::len(rhs));
        for (auto val : rhs) {
          self.texcoord->push_back(py::cast<float>(val));
      }
    }, py::return_value_policy::reference_internal);
  mjsFlex.def_property(
      "info",
      [](raw::MjsFlex& self) -> std::string_view {
        return *self.info;
      },
      [](raw::MjsFlex& self, std::string_view info) {
        *(self.info) = info;
    });

  // mjsMesh
  mjsMesh.def_property(
      "name",
      [](raw::MjsMesh& self) -> std::string_view {
        return *self.name;
      },
      [](raw::MjsMesh& self, std::string_view name) {
        *(self.name) = name;
    });
  mjsMesh.def_property(
      "content_type",
      [](raw::MjsMesh& self) -> std::string_view {
        return *self.content_type;
      },
      [](raw::MjsMesh& self, std::string_view content_type) {
        *(self.content_type) = content_type;
    });
  mjsMesh.def_property(
      "file",
      [](raw::MjsMesh& self) -> std::string_view {
        return *self.file;
      },
      [](raw::MjsMesh& self, std::string_view file) {
        *(self.file) = file;
    });
  mjsMesh.def_property(
      "refpos",
      [](raw::MjsMesh& self) -> MjDouble3 {
        return MjDouble3(self.refpos);
    },
      [](raw::MjsMesh& self, MjDoubleRef3 refpos) {
        MjDouble3(self.refpos) = refpos;
    }, py::return_value_policy::reference_internal);
  mjsMesh.def_property(
      "refquat",
      [](raw::MjsMesh& self) -> MjDouble4 {
        return MjDouble4(self.refquat);
    },
      [](raw::MjsMesh& self, MjDoubleRef4 refquat) {
        MjDouble4(self.refquat) = refquat;
    }, py::return_value_policy::reference_internal);
  mjsMesh.def_property(
      "scale",
      [](raw::MjsMesh& self) -> MjDouble3 {
        return MjDouble3(self.scale);
    },
      [](raw::MjsMesh& self, MjDoubleRef3 scale) {
        MjDouble3(self.scale) = scale;
    }, py::return_value_policy::reference_internal);
mjsMesh.def_property("inertia",[](raw::MjsMesh& self) -> mjtMeshInertia& {
        return self.inertia;
      },[](raw::MjsMesh& self, mjtMeshInertia& inertia) {
        self.inertia = inertia;
      },py::return_value_policy::reference_internal);
mjsMesh.def_property("smoothnormal",[](raw::MjsMesh& self) -> mjtByte {
        return self.smoothnormal;
      },[](raw::MjsMesh& self, mjtByte smoothnormal) {
        self.smoothnormal = smoothnormal;
      });
mjsMesh.def_property("maxhullvert",[](raw::MjsMesh& self) -> int {
        return self.maxhullvert;
      },[](raw::MjsMesh& self, int maxhullvert) {
        self.maxhullvert = maxhullvert;
      });
  mjsMesh.def_property(
    "uservert",
    [](raw::MjsMesh& self) -> py::array_t<float> {
        return py::array_t<float>(self.uservert->size(),
                                      self.uservert->data());
      },
    [](raw::MjsMesh& self, py::object rhs) {
        self.uservert->clear();
        self.uservert->reserve(py::len(rhs));
        for (auto val : rhs) {
          self.uservert->push_back(py::cast<float>(val));
      }
    }, py::return_value_policy::reference_internal);
  mjsMesh.def_property(
    "usernormal",
    [](raw::MjsMesh& self) -> py::array_t<float> {
        return py::array_t<float>(self.usernormal->size(),
                                      self.usernormal->data());
      },
    [](raw::MjsMesh& self, py::object rhs) {
        self.usernormal->clear();
        self.usernormal->reserve(py::len(rhs));
        for (auto val : rhs) {
          self.usernormal->push_back(py::cast<float>(val));
      }
    }, py::return_value_policy::reference_internal);
  mjsMesh.def_property(
    "usertexcoord",
    [](raw::MjsMesh& self) -> py::array_t<float> {
        return py::array_t<float>(self.usertexcoord->size(),
                                      self.usertexcoord->data());
      },
    [](raw::MjsMesh& self, py::object rhs) {
        self.usertexcoord->clear();
        self.usertexcoord->reserve(py::len(rhs));
        for (auto val : rhs) {
          self.usertexcoord->push_back(py::cast<float>(val));
      }
    }, py::return_value_policy::reference_internal);
  mjsMesh.def_property(
    "userface",
    [](raw::MjsMesh& self) -> py::array_t<int> {
        return py::array_t<int>(self.userface->size(),
                                      self.userface->data());
      },
    [](raw::MjsMesh& self, py::object rhs) {
        self.userface->clear();
        self.userface->reserve(py::len(rhs));
        for (auto val : rhs) {
          self.userface->push_back(py::cast<int>(val));
      }
    }, py::return_value_policy::reference_internal);
  mjsMesh.def_property(
    "userfacetexcoord",
    [](raw::MjsMesh& self) -> py::array_t<int> {
        return py::array_t<int>(self.userfacetexcoord->size(),
                                      self.userfacetexcoord->data());
      },
    [](raw::MjsMesh& self, py::object rhs) {
        self.userfacetexcoord->clear();
        self.userfacetexcoord->reserve(py::len(rhs));
        for (auto val : rhs) {
          self.userfacetexcoord->push_back(py::cast<int>(val));
      }
    }, py::return_value_policy::reference_internal);
mjsMesh.def_property("plugin",[](raw::MjsMesh& self) -> raw::MjsPlugin& {
        return self.plugin;
      },[](raw::MjsMesh& self, raw::MjsPlugin& plugin) {
        self.plugin = plugin;
      },py::return_value_policy::reference_internal);
  mjsMesh.def_property(
      "info",
      [](raw::MjsMesh& self) -> std::string_view {
        return *self.info;
      },
      [](raw::MjsMesh& self, std::string_view info) {
        *(self.info) = info;
    });

  // mjsHField
  mjsHField.def_property(
      "name",
      [](raw::MjsHField& self) -> std::string_view {
        return *self.name;
      },
      [](raw::MjsHField& self, std::string_view name) {
        *(self.name) = name;
    });
  mjsHField.def_property(
      "content_type",
      [](raw::MjsHField& self) -> std::string_view {
        return *self.content_type;
      },
      [](raw::MjsHField& self, std::string_view content_type) {
        *(self.content_type) = content_type;
    });
  mjsHField.def_property(
      "file",
      [](raw::MjsHField& self) -> std::string_view {
        return *self.file;
      },
      [](raw::MjsHField& self, std::string_view file) {
        *(self.file) = file;
    });
  mjsHField.def_property(
      "size",
      [](raw::MjsHField& self) -> MjDouble4 {
        return MjDouble4(self.size);
    },
      [](raw::MjsHField& self, MjDoubleRef4 size) {
        MjDouble4(self.size) = size;
    }, py::return_value_policy::reference_internal);
mjsHField.def_property("nrow",[](raw::MjsHField& self) -> int {
        return self.nrow;
      },[](raw::MjsHField& self, int nrow) {
        self.nrow = nrow;
      });
mjsHField.def_property("ncol",[](raw::MjsHField& self) -> int {
        return self.ncol;
      },[](raw::MjsHField& self, int ncol) {
        self.ncol = ncol;
      });
  mjsHField.def_property(
    "userdata",
    [](raw::MjsHField& self) -> py::array_t<float> {
        return py::array_t<float>(self.userdata->size(),
                                      self.userdata->data());
      },
    [](raw::MjsHField& self, py::object rhs) {
        self.userdata->clear();
        self.userdata->reserve(py::len(rhs));
        for (auto val : rhs) {
          self.userdata->push_back(py::cast<float>(val));
      }
    }, py::return_value_policy::reference_internal);
  mjsHField.def_property(
      "info",
      [](raw::MjsHField& self) -> std::string_view {
        return *self.info;
      },
      [](raw::MjsHField& self, std::string_view info) {
        *(self.info) = info;
    });

  // mjsSkin
  mjsSkin.def_property(
      "name",
      [](raw::MjsSkin& self) -> std::string_view {
        return *self.name;
      },
      [](raw::MjsSkin& self, std::string_view name) {
        *(self.name) = name;
    });
  mjsSkin.def_property(
      "file",
      [](raw::MjsSkin& self) -> std::string_view {
        return *self.file;
      },
      [](raw::MjsSkin& self, std::string_view file) {
        *(self.file) = file;
    });
  mjsSkin.def_property(
      "material",
      [](raw::MjsSkin& self) -> std::string_view {
        return *self.material;
      },
      [](raw::MjsSkin& self, std::string_view material) {
        *(self.material) = material;
    });
  mjsSkin.def_property(
      "rgba",
      [](raw::MjsSkin& self) -> MjFloat4 {
        return MjFloat4(self.rgba);
    },
      [](raw::MjsSkin& self, MjFloatRef4 rgba) {
        MjFloat4(self.rgba) = rgba;
    }, py::return_value_policy::reference_internal);
mjsSkin.def_property("inflate",[](raw::MjsSkin& self) -> float {
        return self.inflate;
      },[](raw::MjsSkin& self, float inflate) {
        self.inflate = inflate;
      });
mjsSkin.def_property("group",[](raw::MjsSkin& self) -> int {
        return self.group;
      },[](raw::MjsSkin& self, int group) {
        self.group = group;
      });
  mjsSkin.def_property(
    "vert",
    [](raw::MjsSkin& self) -> py::array_t<float> {
        return py::array_t<float>(self.vert->size(),
                                      self.vert->data());
      },
    [](raw::MjsSkin& self, py::object rhs) {
        self.vert->clear();
        self.vert->reserve(py::len(rhs));
        for (auto val : rhs) {
          self.vert->push_back(py::cast<float>(val));
      }
    }, py::return_value_policy::reference_internal);
  mjsSkin.def_property(
    "texcoord",
    [](raw::MjsSkin& self) -> py::array_t<float> {
        return py::array_t<float>(self.texcoord->size(),
                                      self.texcoord->data());
      },
    [](raw::MjsSkin& self, py::object rhs) {
        self.texcoord->clear();
        self.texcoord->reserve(py::len(rhs));
        for (auto val : rhs) {
          self.texcoord->push_back(py::cast<float>(val));
      }
    }, py::return_value_policy::reference_internal);
  mjsSkin.def_property(
    "face",
    [](raw::MjsSkin& self) -> py::array_t<int> {
        return py::array_t<int>(self.face->size(),
                                      self.face->data());
      },
    [](raw::MjsSkin& self, py::object rhs) {
        self.face->clear();
        self.face->reserve(py::len(rhs));
        for (auto val : rhs) {
          self.face->push_back(py::cast<int>(val));
      }
    }, py::return_value_policy::reference_internal);
  mjsSkin.def_property(
    "bodyname",
    [](raw::MjsSkin& self) -> MjTypeVec<std::string> {
        return MjTypeVec<std::string>(self.bodyname->data(),
                                      self.bodyname->size());
      },
    [](raw::MjsSkin& self, py::object rhs) {
        self.bodyname->clear();
        self.bodyname->reserve(py::len(rhs));
        for (auto val : rhs) {
          self.bodyname->push_back(py::cast<std::string>(val));
      }
    }, py::return_value_policy::move);
  mjsSkin.def_property(
    "bindpos",
    [](raw::MjsSkin& self) -> py::array_t<float> {
        return py::array_t<float>(self.bindpos->size(),
                                      self.bindpos->data());
      },
    [](raw::MjsSkin& self, py::object rhs) {
        self.bindpos->clear();
        self.bindpos->reserve(py::len(rhs));
        for (auto val : rhs) {
          self.bindpos->push_back(py::cast<float>(val));
      }
    }, py::return_value_policy::reference_internal);
  mjsSkin.def_property(
    "bindquat",
    [](raw::MjsSkin& self) -> py::array_t<float> {
        return py::array_t<float>(self.bindquat->size(),
                                      self.bindquat->data());
      },
    [](raw::MjsSkin& self, py::object rhs) {
        self.bindquat->clear();
        self.bindquat->reserve(py::len(rhs));
        for (auto val : rhs) {
          self.bindquat->push_back(py::cast<float>(val));
      }
    }, py::return_value_policy::reference_internal);
  mjsSkin.def_property(
    "vertid",
    [](raw::MjsSkin& self) -> py::list {
        py::list list;
        for (auto inner_vec : *self.vertid) {
          py::list inner_list;
          for (auto val : inner_vec) {
            inner_list.append(val);
          }
          list.append(inner_list);
        }
        return list;
      },
    [](raw::MjsSkin& self, py::object rhs) {
        self.vertid->clear();
        self.vertid->reserve(py::len(rhs));
        for (auto inner_list : rhs) {
          auto inner_vec = py::cast<std::vector<int>>(inner_list);
          self.vertid->push_back(inner_vec);
        }
    }, py::return_value_policy::reference_internal);
  mjsSkin.def_property(
    "vertweight",
    [](raw::MjsSkin& self) -> py::list {
        py::list list;
        for (auto inner_vec : *self.vertweight) {
          py::list inner_list;
          for (auto val : inner_vec) {
            inner_list.append(val);
          }
          list.append(inner_list);
        }
        return list;
      },
    [](raw::MjsSkin& self, py::object rhs) {
        self.vertweight->clear();
        self.vertweight->reserve(py::len(rhs));
        for (auto inner_list : rhs) {
          auto inner_vec = py::cast<std::vector<float>>(inner_list);
          self.vertweight->push_back(inner_vec);
        }
    }, py::return_value_policy::reference_internal);
  mjsSkin.def_property(
      "info",
      [](raw::MjsSkin& self) -> std::string_view {
        return *self.info;
      },
      [](raw::MjsSkin& self, std::string_view info) {
        *(self.info) = info;
    });

  // mjsTexture
  mjsTexture.def_property(
      "name",
      [](raw::MjsTexture& self) -> std::string_view {
        return *self.name;
      },
      [](raw::MjsTexture& self, std::string_view name) {
        *(self.name) = name;
    });
mjsTexture.def_property("type",[](raw::MjsTexture& self) -> mjtTexture& {
        return self.type;
      },[](raw::MjsTexture& self, mjtTexture& type) {
        self.type = type;
      },py::return_value_policy::reference_internal);
mjsTexture.def_property("builtin",[](raw::MjsTexture& self) -> int {
        return self.builtin;
      },[](raw::MjsTexture& self, int builtin) {
        self.builtin = builtin;
      });
mjsTexture.def_property("mark",[](raw::MjsTexture& self) -> int {
        return self.mark;
      },[](raw::MjsTexture& self, int mark) {
        self.mark = mark;
      });
  mjsTexture.def_property(
      "rgb1",
      [](raw::MjsTexture& self) -> MjDouble3 {
        return MjDouble3(self.rgb1);
    },
      [](raw::MjsTexture& self, MjDoubleRef3 rgb1) {
        MjDouble3(self.rgb1) = rgb1;
    }, py::return_value_policy::reference_internal);
  mjsTexture.def_property(
      "rgb2",
      [](raw::MjsTexture& self) -> MjDouble3 {
        return MjDouble3(self.rgb2);
    },
      [](raw::MjsTexture& self, MjDoubleRef3 rgb2) {
        MjDouble3(self.rgb2) = rgb2;
    }, py::return_value_policy::reference_internal);
  mjsTexture.def_property(
      "markrgb",
      [](raw::MjsTexture& self) -> MjDouble3 {
        return MjDouble3(self.markrgb);
    },
      [](raw::MjsTexture& self, MjDoubleRef3 markrgb) {
        MjDouble3(self.markrgb) = markrgb;
    }, py::return_value_policy::reference_internal);
mjsTexture.def_property("random",[](raw::MjsTexture& self) -> double {
        return self.random;
      },[](raw::MjsTexture& self, double random) {
        self.random = random;
      });
mjsTexture.def_property("height",[](raw::MjsTexture& self) -> int {
        return self.height;
      },[](raw::MjsTexture& self, int height) {
        self.height = height;
      });
mjsTexture.def_property("width",[](raw::MjsTexture& self) -> int {
        return self.width;
      },[](raw::MjsTexture& self, int width) {
        self.width = width;
      });
mjsTexture.def_property("nchannel",[](raw::MjsTexture& self) -> int {
        return self.nchannel;
      },[](raw::MjsTexture& self, int nchannel) {
        self.nchannel = nchannel;
      });
  mjsTexture.def_property(
      "content_type",
      [](raw::MjsTexture& self) -> std::string_view {
        return *self.content_type;
      },
      [](raw::MjsTexture& self, std::string_view content_type) {
        *(self.content_type) = content_type;
    });
  mjsTexture.def_property(
      "file",
      [](raw::MjsTexture& self) -> std::string_view {
        return *self.file;
      },
      [](raw::MjsTexture& self, std::string_view file) {
        *(self.file) = file;
    });
  mjsTexture.def_property(
      "gridsize",
      [](raw::MjsTexture& self) -> MjInt2 {
        return MjInt2(self.gridsize);
    },
      [](raw::MjsTexture& self, MjIntRef2 gridsize) {
        MjInt2(self.gridsize) = gridsize;
    }, py::return_value_policy::reference_internal);
  mjsTexture.def_property(
    "gridlayout",
    [](raw::MjsTexture& self) -> MjTypeVec<char> {
      return MjTypeVec<char>(self.gridlayout, 13);
    },
    [](raw::MjsTexture& self, py::object rhs) {
      int i = 0;
      for (auto val : rhs) {
        self.gridlayout[i++] = py::cast<char>(val);
      }
    }, py::return_value_policy::move);
  mjsTexture.def_property(
    "cubefiles",
    [](raw::MjsTexture& self) -> MjTypeVec<std::string> {
        return MjTypeVec<std::string>(self.cubefiles->data(),
                                      self.cubefiles->size());
      },
    [](raw::MjsTexture& self, py::object rhs) {
        self.cubefiles->clear();
        self.cubefiles->reserve(py::len(rhs));
        for (auto val : rhs) {
          self.cubefiles->push_back(py::cast<std::string>(val));
      }
    }, py::return_value_policy::move);
  mjsTexture.def_property(
    "data",
    [](raw::MjsTexture& self) -> MjTypeVec<std::byte> {
        return MjTypeVec<std::byte>(self.data->data(),
                                    self.data->size());
      },
    [](raw::MjsTexture& self, py::bytes& rhs) {
        self.data->clear();
        self.data->reserve(py::len(rhs));
        std::string_view rhs_view = py::cast<std::string_view>(rhs);
        for (auto val : rhs_view) {
          self.data->push_back(static_cast<std::byte>(val));
        }
    }, py::return_value_policy::move);
mjsTexture.def_property("hflip",[](raw::MjsTexture& self) -> mjtByte {
        return self.hflip;
      },[](raw::MjsTexture& self, mjtByte hflip) {
        self.hflip = hflip;
      });
mjsTexture.def_property("vflip",[](raw::MjsTexture& self) -> mjtByte {
        return self.vflip;
      },[](raw::MjsTexture& self, mjtByte vflip) {
        self.vflip = vflip;
      });
  mjsTexture.def_property(
      "info",
      [](raw::MjsTexture& self) -> std::string_view {
        return *self.info;
      },
      [](raw::MjsTexture& self, std::string_view info) {
        *(self.info) = info;
    });

  // mjsMaterial
  mjsMaterial.def_property(
      "name",
      [](raw::MjsMaterial& self) -> std::string_view {
        return *self.name;
      },
      [](raw::MjsMaterial& self, std::string_view name) {
        *(self.name) = name;
    });
  mjsMaterial.def_property(
    "textures",
    [](raw::MjsMaterial& self) -> MjTypeVec<std::string> {
        return MjTypeVec<std::string>(self.textures->data(),
                                      self.textures->size());
      },
    [](raw::MjsMaterial& self, py::object rhs) {
        self.textures->clear();
        self.textures->reserve(py::len(rhs));
        for (auto val : rhs) {
          self.textures->push_back(py::cast<std::string>(val));
      }
    }, py::return_value_policy::move);
mjsMaterial.def_property("texuniform",[](raw::MjsMaterial& self) -> mjtByte {
        return self.texuniform;
      },[](raw::MjsMaterial& self, mjtByte texuniform) {
        self.texuniform = texuniform;
      });
  mjsMaterial.def_property(
      "texrepeat",
      [](raw::MjsMaterial& self) -> MjFloat2 {
        return MjFloat2(self.texrepeat);
    },
      [](raw::MjsMaterial& self, MjFloatRef2 texrepeat) {
        MjFloat2(self.texrepeat) = texrepeat;
    }, py::return_value_policy::reference_internal);
mjsMaterial.def_property("emission",[](raw::MjsMaterial& self) -> float {
        return self.emission;
      },[](raw::MjsMaterial& self, float emission) {
        self.emission = emission;
      });
mjsMaterial.def_property("specular",[](raw::MjsMaterial& self) -> float {
        return self.specular;
      },[](raw::MjsMaterial& self, float specular) {
        self.specular = specular;
      });
mjsMaterial.def_property("shininess",[](raw::MjsMaterial& self) -> float {
        return self.shininess;
      },[](raw::MjsMaterial& self, float shininess) {
        self.shininess = shininess;
      });
mjsMaterial.def_property("reflectance",[](raw::MjsMaterial& self) -> float {
        return self.reflectance;
      },[](raw::MjsMaterial& self, float reflectance) {
        self.reflectance = reflectance;
      });
mjsMaterial.def_property("metallic",[](raw::MjsMaterial& self) -> float {
        return self.metallic;
      },[](raw::MjsMaterial& self, float metallic) {
        self.metallic = metallic;
      });
mjsMaterial.def_property("roughness",[](raw::MjsMaterial& self) -> float {
        return self.roughness;
      },[](raw::MjsMaterial& self, float roughness) {
        self.roughness = roughness;
      });
  mjsMaterial.def_property(
      "rgba",
      [](raw::MjsMaterial& self) -> MjFloat4 {
        return MjFloat4(self.rgba);
    },
      [](raw::MjsMaterial& self, MjFloatRef4 rgba) {
        MjFloat4(self.rgba) = rgba;
    }, py::return_value_policy::reference_internal);
  mjsMaterial.def_property(
      "info",
      [](raw::MjsMaterial& self) -> std::string_view {
        return *self.info;
      },
      [](raw::MjsMaterial& self, std::string_view info) {
        *(self.info) = info;
    });

  // mjsPair
  mjsPair.def_property(
      "name",
      [](raw::MjsPair& self) -> std::string_view {
        return *self.name;
      },
      [](raw::MjsPair& self, std::string_view name) {
        *(self.name) = name;
    });
  mjsPair.def_property(
      "geomname1",
      [](raw::MjsPair& self) -> std::string_view {
        return *self.geomname1;
      },
      [](raw::MjsPair& self, std::string_view geomname1) {
        *(self.geomname1) = geomname1;
    });
  mjsPair.def_property(
      "geomname2",
      [](raw::MjsPair& self) -> std::string_view {
        return *self.geomname2;
      },
      [](raw::MjsPair& self, std::string_view geomname2) {
        *(self.geomname2) = geomname2;
    });
mjsPair.def_property("condim",[](raw::MjsPair& self) -> int {
        return self.condim;
      },[](raw::MjsPair& self, int condim) {
        self.condim = condim;
      });
  mjsPair.def_property(
      "solref",
      [](raw::MjsPair& self) -> MjDouble2 {
        return MjDouble2(self.solref);
    },
      [](raw::MjsPair& self, MjDoubleRef2 solref) {
        MjDouble2(self.solref) = solref;
    }, py::return_value_policy::reference_internal);
  mjsPair.def_property(
      "solreffriction",
      [](raw::MjsPair& self) -> MjDouble2 {
        return MjDouble2(self.solreffriction);
    },
      [](raw::MjsPair& self, MjDoubleRef2 solreffriction) {
        MjDouble2(self.solreffriction) = solreffriction;
    }, py::return_value_policy::reference_internal);
  mjsPair.def_property(
      "solimp",
      [](raw::MjsPair& self) -> MjDouble5 {
        return MjDouble5(self.solimp);
    },
      [](raw::MjsPair& self, MjDoubleRef5 solimp) {
        MjDouble5(self.solimp) = solimp;
    }, py::return_value_policy::reference_internal);
mjsPair.def_property("margin",[](raw::MjsPair& self) -> double {
        return self.margin;
      },[](raw::MjsPair& self, double margin) {
        self.margin = margin;
      });
mjsPair.def_property("gap",[](raw::MjsPair& self) -> double {
        return self.gap;
      },[](raw::MjsPair& self, double gap) {
        self.gap = gap;
      });
  mjsPair.def_property(
      "friction",
      [](raw::MjsPair& self) -> MjDouble5 {
        return MjDouble5(self.friction);
    },
      [](raw::MjsPair& self, MjDoubleRef5 friction) {
        MjDouble5(self.friction) = friction;
    }, py::return_value_policy::reference_internal);
  mjsPair.def_property(
      "info",
      [](raw::MjsPair& self) -> std::string_view {
        return *self.info;
      },
      [](raw::MjsPair& self, std::string_view info) {
        *(self.info) = info;
    });

  // mjsExclude
  mjsExclude.def_property(
      "name",
      [](raw::MjsExclude& self) -> std::string_view {
        return *self.name;
      },
      [](raw::MjsExclude& self, std::string_view name) {
        *(self.name) = name;
    });
  mjsExclude.def_property(
      "bodyname1",
      [](raw::MjsExclude& self) -> std::string_view {
        return *self.bodyname1;
      },
      [](raw::MjsExclude& self, std::string_view bodyname1) {
        *(self.bodyname1) = bodyname1;
    });
  mjsExclude.def_property(
      "bodyname2",
      [](raw::MjsExclude& self) -> std::string_view {
        return *self.bodyname2;
      },
      [](raw::MjsExclude& self, std::string_view bodyname2) {
        *(self.bodyname2) = bodyname2;
    });
  mjsExclude.def_property(
      "info",
      [](raw::MjsExclude& self) -> std::string_view {
        return *self.info;
      },
      [](raw::MjsExclude& self, std::string_view info) {
        *(self.info) = info;
    });

  // mjsEquality
  mjsEquality.def_property(
      "name",
      [](raw::MjsEquality& self) -> std::string_view {
        return *self.name;
      },
      [](raw::MjsEquality& self, std::string_view name) {
        *(self.name) = name;
    });
mjsEquality.def_property("type",[](raw::MjsEquality& self) -> mjtEq& {
        return self.type;
      },[](raw::MjsEquality& self, mjtEq& type) {
        self.type = type;
      },py::return_value_policy::reference_internal);
  mjsEquality.def_property(
      "data",
      [](raw::MjsEquality& self) -> MjDouble11 {
        return MjDouble11(self.data);
    },
      [](raw::MjsEquality& self, MjDoubleRef11 data) {
        MjDouble11(self.data) = data;
    }, py::return_value_policy::reference_internal);
mjsEquality.def_property("active",[](raw::MjsEquality& self) -> mjtByte {
        return self.active;
      },[](raw::MjsEquality& self, mjtByte active) {
        self.active = active;
      });
  mjsEquality.def_property(
      "name1",
      [](raw::MjsEquality& self) -> std::string_view {
        return *self.name1;
      },
      [](raw::MjsEquality& self, std::string_view name1) {
        *(self.name1) = name1;
    });
  mjsEquality.def_property(
      "name2",
      [](raw::MjsEquality& self) -> std::string_view {
        return *self.name2;
      },
      [](raw::MjsEquality& self, std::string_view name2) {
        *(self.name2) = name2;
    });
mjsEquality.def_property("objtype",[](raw::MjsEquality& self) -> mjtObj& {
        return self.objtype;
      },[](raw::MjsEquality& self, mjtObj& objtype) {
        self.objtype = objtype;
      },py::return_value_policy::reference_internal);
  mjsEquality.def_property(
      "solref",
      [](raw::MjsEquality& self) -> MjDouble2 {
        return MjDouble2(self.solref);
    },
      [](raw::MjsEquality& self, MjDoubleRef2 solref) {
        MjDouble2(self.solref) = solref;
    }, py::return_value_policy::reference_internal);
  mjsEquality.def_property(
      "solimp",
      [](raw::MjsEquality& self) -> MjDouble5 {
        return MjDouble5(self.solimp);
    },
      [](raw::MjsEquality& self, MjDoubleRef5 solimp) {
        MjDouble5(self.solimp) = solimp;
    }, py::return_value_policy::reference_internal);
  mjsEquality.def_property(
      "info",
      [](raw::MjsEquality& self) -> std::string_view {
        return *self.info;
      },
      [](raw::MjsEquality& self, std::string_view info) {
        *(self.info) = info;
    });

  // mjsTendon
  mjsTendon.def_property(
      "name",
      [](raw::MjsTendon& self) -> std::string_view {
        return *self.name;
      },
      [](raw::MjsTendon& self, std::string_view name) {
        *(self.name) = name;
    });
mjsTendon.def_property("stiffness",[](raw::MjsTendon& self) -> double {
        return self.stiffness;
      },[](raw::MjsTendon& self, double stiffness) {
        self.stiffness = stiffness;
      });
  mjsTendon.def_property(
      "springlength",
      [](raw::MjsTendon& self) -> MjDouble2 {
        return MjDouble2(self.springlength);
    },
      [](raw::MjsTendon& self, MjDoubleRef2 springlength) {
        MjDouble2(self.springlength) = springlength;
    }, py::return_value_policy::reference_internal);
mjsTendon.def_property("damping",[](raw::MjsTendon& self) -> double {
        return self.damping;
      },[](raw::MjsTendon& self, double damping) {
        self.damping = damping;
      });
mjsTendon.def_property("frictionloss",[](raw::MjsTendon& self) -> double {
        return self.frictionloss;
      },[](raw::MjsTendon& self, double frictionloss) {
        self.frictionloss = frictionloss;
      });
  mjsTendon.def_property(
      "solref_friction",
      [](raw::MjsTendon& self) -> MjDouble2 {
        return MjDouble2(self.solref_friction);
    },
      [](raw::MjsTendon& self, MjDoubleRef2 solref_friction) {
        MjDouble2(self.solref_friction) = solref_friction;
    }, py::return_value_policy::reference_internal);
  mjsTendon.def_property(
      "solimp_friction",
      [](raw::MjsTendon& self) -> MjDouble5 {
        return MjDouble5(self.solimp_friction);
    },
      [](raw::MjsTendon& self, MjDoubleRef5 solimp_friction) {
        MjDouble5(self.solimp_friction) = solimp_friction;
    }, py::return_value_policy::reference_internal);
mjsTendon.def_property("limited",[](raw::MjsTendon& self) -> int {
        return self.limited;
      },[](raw::MjsTendon& self, int limited) {
        self.limited = limited;
      });
  mjsTendon.def_property(
      "range",
      [](raw::MjsTendon& self) -> MjDouble2 {
        return MjDouble2(self.range);
    },
      [](raw::MjsTendon& self, MjDoubleRef2 range) {
        MjDouble2(self.range) = range;
    }, py::return_value_policy::reference_internal);
mjsTendon.def_property("margin",[](raw::MjsTendon& self) -> double {
        return self.margin;
      },[](raw::MjsTendon& self, double margin) {
        self.margin = margin;
      });
  mjsTendon.def_property(
      "solref_limit",
      [](raw::MjsTendon& self) -> MjDouble2 {
        return MjDouble2(self.solref_limit);
    },
      [](raw::MjsTendon& self, MjDoubleRef2 solref_limit) {
        MjDouble2(self.solref_limit) = solref_limit;
    }, py::return_value_policy::reference_internal);
  mjsTendon.def_property(
      "solimp_limit",
      [](raw::MjsTendon& self) -> MjDouble5 {
        return MjDouble5(self.solimp_limit);
    },
      [](raw::MjsTendon& self, MjDoubleRef5 solimp_limit) {
        MjDouble5(self.solimp_limit) = solimp_limit;
    }, py::return_value_policy::reference_internal);
  mjsTendon.def_property(
      "material",
      [](raw::MjsTendon& self) -> std::string_view {
        return *self.material;
      },
      [](raw::MjsTendon& self, std::string_view material) {
        *(self.material) = material;
    });
mjsTendon.def_property("width",[](raw::MjsTendon& self) -> double {
        return self.width;
      },[](raw::MjsTendon& self, double width) {
        self.width = width;
      });
  mjsTendon.def_property(
      "rgba",
      [](raw::MjsTendon& self) -> MjFloat4 {
        return MjFloat4(self.rgba);
    },
      [](raw::MjsTendon& self, MjFloatRef4 rgba) {
        MjFloat4(self.rgba) = rgba;
    }, py::return_value_policy::reference_internal);
mjsTendon.def_property("group",[](raw::MjsTendon& self) -> int {
        return self.group;
      },[](raw::MjsTendon& self, int group) {
        self.group = group;
      });
  mjsTendon.def_property(
    "userdata",
    [](raw::MjsTendon& self) -> py::array_t<double> {
        return py::array_t<double>(self.userdata->size(),
                                      self.userdata->data());
      },
    [](raw::MjsTendon& self, py::object rhs) {
        self.userdata->clear();
        self.userdata->reserve(py::len(rhs));
        for (auto val : rhs) {
          self.userdata->push_back(py::cast<double>(val));
      }
    }, py::return_value_policy::reference_internal);
  mjsTendon.def_property(
      "info",
      [](raw::MjsTendon& self) -> std::string_view {
        return *self.info;
      },
      [](raw::MjsTendon& self, std::string_view info) {
        *(self.info) = info;
    });

  // mjsWrap
  mjsWrap.def_property(
      "info",
      [](raw::MjsWrap& self) -> std::string_view {
        return *self.info;
      },
      [](raw::MjsWrap& self, std::string_view info) {
        *(self.info) = info;
    });

  // mjsActuator
  mjsActuator.def_property(
      "name",
      [](raw::MjsActuator& self) -> std::string_view {
        return *self.name;
      },
      [](raw::MjsActuator& self, std::string_view name) {
        *(self.name) = name;
    });
mjsActuator.def_property("gaintype",[](raw::MjsActuator& self) -> mjtGain& {
        return self.gaintype;
      },[](raw::MjsActuator& self, mjtGain& gaintype) {
        self.gaintype = gaintype;
      },py::return_value_policy::reference_internal);
  mjsActuator.def_property(
      "gainprm",
      [](raw::MjsActuator& self) -> MjDouble10 {
        return MjDouble10(self.gainprm);
    },
      [](raw::MjsActuator& self, MjDoubleRef10 gainprm) {
        MjDouble10(self.gainprm) = gainprm;
    }, py::return_value_policy::reference_internal);
mjsActuator.def_property("biastype",[](raw::MjsActuator& self) -> mjtBias& {
        return self.biastype;
      },[](raw::MjsActuator& self, mjtBias& biastype) {
        self.biastype = biastype;
      },py::return_value_policy::reference_internal);
  mjsActuator.def_property(
      "biasprm",
      [](raw::MjsActuator& self) -> MjDouble10 {
        return MjDouble10(self.biasprm);
    },
      [](raw::MjsActuator& self, MjDoubleRef10 biasprm) {
        MjDouble10(self.biasprm) = biasprm;
    }, py::return_value_policy::reference_internal);
mjsActuator.def_property("dyntype",[](raw::MjsActuator& self) -> mjtDyn& {
        return self.dyntype;
      },[](raw::MjsActuator& self, mjtDyn& dyntype) {
        self.dyntype = dyntype;
      },py::return_value_policy::reference_internal);
  mjsActuator.def_property(
      "dynprm",
      [](raw::MjsActuator& self) -> MjDouble10 {
        return MjDouble10(self.dynprm);
    },
      [](raw::MjsActuator& self, MjDoubleRef10 dynprm) {
        MjDouble10(self.dynprm) = dynprm;
    }, py::return_value_policy::reference_internal);
mjsActuator.def_property("actdim",[](raw::MjsActuator& self) -> int {
        return self.actdim;
      },[](raw::MjsActuator& self, int actdim) {
        self.actdim = actdim;
      });
mjsActuator.def_property("actearly",[](raw::MjsActuator& self) -> mjtByte {
        return self.actearly;
      },[](raw::MjsActuator& self, mjtByte actearly) {
        self.actearly = actearly;
      });
mjsActuator.def_property("trntype",[](raw::MjsActuator& self) -> mjtTrn& {
        return self.trntype;
      },[](raw::MjsActuator& self, mjtTrn& trntype) {
        self.trntype = trntype;
      },py::return_value_policy::reference_internal);
  mjsActuator.def_property(
      "gear",
      [](raw::MjsActuator& self) -> MjDouble6 {
        return MjDouble6(self.gear);
    },
      [](raw::MjsActuator& self, MjDoubleRef6 gear) {
        MjDouble6(self.gear) = gear;
    }, py::return_value_policy::reference_internal);
  mjsActuator.def_property(
      "target",
      [](raw::MjsActuator& self) -> std::string_view {
        return *self.target;
      },
      [](raw::MjsActuator& self, std::string_view target) {
        *(self.target) = target;
    });
  mjsActuator.def_property(
      "refsite",
      [](raw::MjsActuator& self) -> std::string_view {
        return *self.refsite;
      },
      [](raw::MjsActuator& self, std::string_view refsite) {
        *(self.refsite) = refsite;
    });
  mjsActuator.def_property(
      "slidersite",
      [](raw::MjsActuator& self) -> std::string_view {
        return *self.slidersite;
      },
      [](raw::MjsActuator& self, std::string_view slidersite) {
        *(self.slidersite) = slidersite;
    });
mjsActuator.def_property("cranklength",[](raw::MjsActuator& self) -> double {
        return self.cranklength;
      },[](raw::MjsActuator& self, double cranklength) {
        self.cranklength = cranklength;
      });
  mjsActuator.def_property(
      "lengthrange",
      [](raw::MjsActuator& self) -> MjDouble2 {
        return MjDouble2(self.lengthrange);
    },
      [](raw::MjsActuator& self, MjDoubleRef2 lengthrange) {
        MjDouble2(self.lengthrange) = lengthrange;
    }, py::return_value_policy::reference_internal);
mjsActuator.def_property("inheritrange",[](raw::MjsActuator& self) -> double {
        return self.inheritrange;
      },[](raw::MjsActuator& self, double inheritrange) {
        self.inheritrange = inheritrange;
      });
mjsActuator.def_property("ctrllimited",[](raw::MjsActuator& self) -> int {
        return self.ctrllimited;
      },[](raw::MjsActuator& self, int ctrllimited) {
        self.ctrllimited = ctrllimited;
      });
  mjsActuator.def_property(
      "ctrlrange",
      [](raw::MjsActuator& self) -> MjDouble2 {
        return MjDouble2(self.ctrlrange);
    },
      [](raw::MjsActuator& self, MjDoubleRef2 ctrlrange) {
        MjDouble2(self.ctrlrange) = ctrlrange;
    }, py::return_value_policy::reference_internal);
mjsActuator.def_property("forcelimited",[](raw::MjsActuator& self) -> int {
        return self.forcelimited;
      },[](raw::MjsActuator& self, int forcelimited) {
        self.forcelimited = forcelimited;
      });
  mjsActuator.def_property(
      "forcerange",
      [](raw::MjsActuator& self) -> MjDouble2 {
        return MjDouble2(self.forcerange);
    },
      [](raw::MjsActuator& self, MjDoubleRef2 forcerange) {
        MjDouble2(self.forcerange) = forcerange;
    }, py::return_value_policy::reference_internal);
mjsActuator.def_property("actlimited",[](raw::MjsActuator& self) -> int {
        return self.actlimited;
      },[](raw::MjsActuator& self, int actlimited) {
        self.actlimited = actlimited;
      });
  mjsActuator.def_property(
      "actrange",
      [](raw::MjsActuator& self) -> MjDouble2 {
        return MjDouble2(self.actrange);
    },
      [](raw::MjsActuator& self, MjDoubleRef2 actrange) {
        MjDouble2(self.actrange) = actrange;
    }, py::return_value_policy::reference_internal);
mjsActuator.def_property("group",[](raw::MjsActuator& self) -> int {
        return self.group;
      },[](raw::MjsActuator& self, int group) {
        self.group = group;
      });
  mjsActuator.def_property(
    "userdata",
    [](raw::MjsActuator& self) -> py::array_t<double> {
        return py::array_t<double>(self.userdata->size(),
                                      self.userdata->data());
      },
    [](raw::MjsActuator& self, py::object rhs) {
        self.userdata->clear();
        self.userdata->reserve(py::len(rhs));
        for (auto val : rhs) {
          self.userdata->push_back(py::cast<double>(val));
      }
    }, py::return_value_policy::reference_internal);
mjsActuator.def_property("plugin",[](raw::MjsActuator& self) -> raw::MjsPlugin& {
        return self.plugin;
      },[](raw::MjsActuator& self, raw::MjsPlugin& plugin) {
        self.plugin = plugin;
      },py::return_value_policy::reference_internal);
  mjsActuator.def_property(
      "info",
      [](raw::MjsActuator& self) -> std::string_view {
        return *self.info;
      },
      [](raw::MjsActuator& self, std::string_view info) {
        *(self.info) = info;
    });

  // mjsSensor
  mjsSensor.def_property(
      "name",
      [](raw::MjsSensor& self) -> std::string_view {
        return *self.name;
      },
      [](raw::MjsSensor& self, std::string_view name) {
        *(self.name) = name;
    });
mjsSensor.def_property("type",[](raw::MjsSensor& self) -> mjtSensor& {
        return self.type;
      },[](raw::MjsSensor& self, mjtSensor& type) {
        self.type = type;
      },py::return_value_policy::reference_internal);
mjsSensor.def_property("objtype",[](raw::MjsSensor& self) -> mjtObj& {
        return self.objtype;
      },[](raw::MjsSensor& self, mjtObj& objtype) {
        self.objtype = objtype;
      },py::return_value_policy::reference_internal);
  mjsSensor.def_property(
      "objname",
      [](raw::MjsSensor& self) -> std::string_view {
        return *self.objname;
      },
      [](raw::MjsSensor& self, std::string_view objname) {
        *(self.objname) = objname;
    });
mjsSensor.def_property("reftype",[](raw::MjsSensor& self) -> mjtObj& {
        return self.reftype;
      },[](raw::MjsSensor& self, mjtObj& reftype) {
        self.reftype = reftype;
      },py::return_value_policy::reference_internal);
  mjsSensor.def_property(
      "refname",
      [](raw::MjsSensor& self) -> std::string_view {
        return *self.refname;
      },
      [](raw::MjsSensor& self, std::string_view refname) {
        *(self.refname) = refname;
    });
mjsSensor.def_property("datatype",[](raw::MjsSensor& self) -> mjtDataType& {
        return self.datatype;
      },[](raw::MjsSensor& self, mjtDataType& datatype) {
        self.datatype = datatype;
      },py::return_value_policy::reference_internal);
mjsSensor.def_property("needstage",[](raw::MjsSensor& self) -> mjtStage& {
        return self.needstage;
      },[](raw::MjsSensor& self, mjtStage& needstage) {
        self.needstage = needstage;
      },py::return_value_policy::reference_internal);
mjsSensor.def_property("dim",[](raw::MjsSensor& self) -> int {
        return self.dim;
      },[](raw::MjsSensor& self, int dim) {
        self.dim = dim;
      });
mjsSensor.def_property("cutoff",[](raw::MjsSensor& self) -> double {
        return self.cutoff;
      },[](raw::MjsSensor& self, double cutoff) {
        self.cutoff = cutoff;
      });
mjsSensor.def_property("noise",[](raw::MjsSensor& self) -> double {
        return self.noise;
      },[](raw::MjsSensor& self, double noise) {
        self.noise = noise;
      });
  mjsSensor.def_property(
    "userdata",
    [](raw::MjsSensor& self) -> py::array_t<double> {
        return py::array_t<double>(self.userdata->size(),
                                      self.userdata->data());
      },
    [](raw::MjsSensor& self, py::object rhs) {
        self.userdata->clear();
        self.userdata->reserve(py::len(rhs));
        for (auto val : rhs) {
          self.userdata->push_back(py::cast<double>(val));
      }
    }, py::return_value_policy::reference_internal);
mjsSensor.def_property("plugin",[](raw::MjsSensor& self) -> raw::MjsPlugin& {
        return self.plugin;
      },[](raw::MjsSensor& self, raw::MjsPlugin& plugin) {
        self.plugin = plugin;
      },py::return_value_policy::reference_internal);
  mjsSensor.def_property(
      "info",
      [](raw::MjsSensor& self) -> std::string_view {
        return *self.info;
      },
      [](raw::MjsSensor& self, std::string_view info) {
        *(self.info) = info;
    });

  // mjsNumeric
  mjsNumeric.def_property(
      "name",
      [](raw::MjsNumeric& self) -> std::string_view {
        return *self.name;
      },
      [](raw::MjsNumeric& self, std::string_view name) {
        *(self.name) = name;
    });
  mjsNumeric.def_property(
    "data",
    [](raw::MjsNumeric& self) -> py::array_t<double> {
        return py::array_t<double>(self.data->size(),
                                      self.data->data());
      },
    [](raw::MjsNumeric& self, py::object rhs) {
        self.data->clear();
        self.data->reserve(py::len(rhs));
        for (auto val : rhs) {
          self.data->push_back(py::cast<double>(val));
      }
    }, py::return_value_policy::reference_internal);
mjsNumeric.def_property("size",[](raw::MjsNumeric& self) -> int {
        return self.size;
      },[](raw::MjsNumeric& self, int size) {
        self.size = size;
      });
  mjsNumeric.def_property(
      "info",
      [](raw::MjsNumeric& self) -> std::string_view {
        return *self.info;
      },
      [](raw::MjsNumeric& self, std::string_view info) {
        *(self.info) = info;
    });

  // mjsText
  mjsText.def_property(
      "name",
      [](raw::MjsText& self) -> std::string_view {
        return *self.name;
      },
      [](raw::MjsText& self, std::string_view name) {
        *(self.name) = name;
    });
  mjsText.def_property(
      "data",
      [](raw::MjsText& self) -> std::string_view {
        return *self.data;
      },
      [](raw::MjsText& self, std::string_view data) {
        *(self.data) = data;
    });
  mjsText.def_property(
      "info",
      [](raw::MjsText& self) -> std::string_view {
        return *self.info;
      },
      [](raw::MjsText& self, std::string_view info) {
        *(self.info) = info;
    });

  // mjsTuple
  mjsTuple.def_property(
      "name",
      [](raw::MjsTuple& self) -> std::string_view {
        return *self.name;
      },
      [](raw::MjsTuple& self, std::string_view name) {
        *(self.name) = name;
    });
  mjsTuple.def_property(
    "objtype",
    [](raw::MjsTuple& self) -> py::array_t<int> {
        return py::array_t<int>(self.objtype->size(),
                                      self.objtype->data());
      },
    [](raw::MjsTuple& self, py::object rhs) {
        self.objtype->clear();
        self.objtype->reserve(py::len(rhs));
        for (auto val : rhs) {
          self.objtype->push_back(py::cast<int>(val));
      }
    }, py::return_value_policy::reference_internal);
  mjsTuple.def_property(
    "objname",
    [](raw::MjsTuple& self) -> MjTypeVec<std::string> {
        return MjTypeVec<std::string>(self.objname->data(),
                                      self.objname->size());
      },
    [](raw::MjsTuple& self, py::object rhs) {
        self.objname->clear();
        self.objname->reserve(py::len(rhs));
        for (auto val : rhs) {
          self.objname->push_back(py::cast<std::string>(val));
      }
    }, py::return_value_policy::move);
  mjsTuple.def_property(
    "objprm",
    [](raw::MjsTuple& self) -> py::array_t<double> {
        return py::array_t<double>(self.objprm->size(),
                                      self.objprm->data());
      },
    [](raw::MjsTuple& self, py::object rhs) {
        self.objprm->clear();
        self.objprm->reserve(py::len(rhs));
        for (auto val : rhs) {
          self.objprm->push_back(py::cast<double>(val));
      }
    }, py::return_value_policy::reference_internal);
  mjsTuple.def_property(
      "info",
      [](raw::MjsTuple& self) -> std::string_view {
        return *self.info;
      },
      [](raw::MjsTuple& self, std::string_view info) {
        *(self.info) = info;
    });

  // mjsKey
  mjsKey.def_property(
      "name",
      [](raw::MjsKey& self) -> std::string_view {
        return *self.name;
      },
      [](raw::MjsKey& self, std::string_view name) {
        *(self.name) = name;
    });
mjsKey.def_property("time",[](raw::MjsKey& self) -> double {
        return self.time;
      },[](raw::MjsKey& self, double time) {
        self.time = time;
      });
  mjsKey.def_property(
    "qpos",
    [](raw::MjsKey& self) -> py::array_t<double> {
        return py::array_t<double>(self.qpos->size(),
                                      self.qpos->data());
      },
    [](raw::MjsKey& self, py::object rhs) {
        self.qpos->clear();
        self.qpos->reserve(py::len(rhs));
        for (auto val : rhs) {
          self.qpos->push_back(py::cast<double>(val));
      }
    }, py::return_value_policy::reference_internal);
  mjsKey.def_property(
    "qvel",
    [](raw::MjsKey& self) -> py::array_t<double> {
        return py::array_t<double>(self.qvel->size(),
                                      self.qvel->data());
      },
    [](raw::MjsKey& self, py::object rhs) {
        self.qvel->clear();
        self.qvel->reserve(py::len(rhs));
        for (auto val : rhs) {
          self.qvel->push_back(py::cast<double>(val));
      }
    }, py::return_value_policy::reference_internal);
  mjsKey.def_property(
    "act",
    [](raw::MjsKey& self) -> py::array_t<double> {
        return py::array_t<double>(self.act->size(),
                                      self.act->data());
      },
    [](raw::MjsKey& self, py::object rhs) {
        self.act->clear();
        self.act->reserve(py::len(rhs));
        for (auto val : rhs) {
          self.act->push_back(py::cast<double>(val));
      }
    }, py::return_value_policy::reference_internal);
  mjsKey.def_property(
    "mpos",
    [](raw::MjsKey& self) -> py::array_t<double> {
        return py::array_t<double>(self.mpos->size(),
                                      self.mpos->data());
      },
    [](raw::MjsKey& self, py::object rhs) {
        self.mpos->clear();
        self.mpos->reserve(py::len(rhs));
        for (auto val : rhs) {
          self.mpos->push_back(py::cast<double>(val));
      }
    }, py::return_value_policy::reference_internal);
  mjsKey.def_property(
    "mquat",
    [](raw::MjsKey& self) -> py::array_t<double> {
        return py::array_t<double>(self.mquat->size(),
                                      self.mquat->data());
      },
    [](raw::MjsKey& self, py::object rhs) {
        self.mquat->clear();
        self.mquat->reserve(py::len(rhs));
        for (auto val : rhs) {
          self.mquat->push_back(py::cast<double>(val));
      }
    }, py::return_value_policy::reference_internal);
  mjsKey.def_property(
    "ctrl",
    [](raw::MjsKey& self) -> py::array_t<double> {
        return py::array_t<double>(self.ctrl->size(),
                                      self.ctrl->data());
      },
    [](raw::MjsKey& self, py::object rhs) {
        self.ctrl->clear();
        self.ctrl->reserve(py::len(rhs));
        for (auto val : rhs) {
          self.ctrl->push_back(py::cast<double>(val));
      }
    }, py::return_value_policy::reference_internal);
  mjsKey.def_property(
      "info",
      [](raw::MjsKey& self) -> std::string_view {
        return *self.info;
      },
      [](raw::MjsKey& self, std::string_view info) {
        *(self.info) = info;
    });

  // mjsDefault
  mjsDefault.def_property(
      "name",
      [](raw::MjsDefault& self) -> std::string_view {
        return *self.name;
      },
      [](raw::MjsDefault& self, std::string_view name) {
        *(self.name) = name;
    });
mjsDefault.def_property("joint",[](raw::MjsDefault& self) -> raw::MjsJoint* {
        return self.joint;
      },[](raw::MjsDefault& self, raw::MjsJoint* joint) {
        self.joint = joint;
      },py::return_value_policy::reference_internal);
mjsDefault.def_property("geom",[](raw::MjsDefault& self) -> raw::MjsGeom* {
        return self.geom;
      },[](raw::MjsDefault& self, raw::MjsGeom* geom) {
        self.geom = geom;
      },py::return_value_policy::reference_internal);
mjsDefault.def_property("site",[](raw::MjsDefault& self) -> raw::MjsSite* {
        return self.site;
      },[](raw::MjsDefault& self, raw::MjsSite* site) {
        self.site = site;
      },py::return_value_policy::reference_internal);
mjsDefault.def_property("camera",[](raw::MjsDefault& self) -> raw::MjsCamera* {
        return self.camera;
      },[](raw::MjsDefault& self, raw::MjsCamera* camera) {
        self.camera = camera;
      },py::return_value_policy::reference_internal);
mjsDefault.def_property("light",[](raw::MjsDefault& self) -> raw::MjsLight* {
        return self.light;
      },[](raw::MjsDefault& self, raw::MjsLight* light) {
        self.light = light;
      },py::return_value_policy::reference_internal);
mjsDefault.def_property("flex",[](raw::MjsDefault& self) -> raw::MjsFlex* {
        return self.flex;
      },[](raw::MjsDefault& self, raw::MjsFlex* flex) {
        self.flex = flex;
      },py::return_value_policy::reference_internal);
mjsDefault.def_property("mesh",[](raw::MjsDefault& self) -> raw::MjsMesh* {
        return self.mesh;
      },[](raw::MjsDefault& self, raw::MjsMesh* mesh) {
        self.mesh = mesh;
      },py::return_value_policy::reference_internal);
mjsDefault.def_property("material",[](raw::MjsDefault& self) -> raw::MjsMaterial* {
        return self.material;
      },[](raw::MjsDefault& self, raw::MjsMaterial* material) {
        self.material = material;
      },py::return_value_policy::reference_internal);
mjsDefault.def_property("pair",[](raw::MjsDefault& self) -> raw::MjsPair* {
        return self.pair;
      },[](raw::MjsDefault& self, raw::MjsPair* pair) {
        self.pair = pair;
      },py::return_value_policy::reference_internal);
mjsDefault.def_property("equality",[](raw::MjsDefault& self) -> raw::MjsEquality* {
        return self.equality;
      },[](raw::MjsDefault& self, raw::MjsEquality* equality) {
        self.equality = equality;
      },py::return_value_policy::reference_internal);
mjsDefault.def_property("tendon",[](raw::MjsDefault& self) -> raw::MjsTendon* {
        return self.tendon;
      },[](raw::MjsDefault& self, raw::MjsTendon* tendon) {
        self.tendon = tendon;
      },py::return_value_policy::reference_internal);
mjsDefault.def_property("actuator",[](raw::MjsDefault& self) -> raw::MjsActuator* {
        return self.actuator;
      },[](raw::MjsDefault& self, raw::MjsActuator* actuator) {
        self.actuator = actuator;
      },py::return_value_policy::reference_internal);

          mjsBody.def("add_body", [](raw::MjsBody& self,
            raw::MjsDefault* default_, py::kwargs kwargs) -> raw::MjsBody* {
            auto out = mjs_addBody(&self, default_);
        
        std::set<std::string> valid_kwargs = {"name", "childclass", "pos", "quat", "axisangle", "xyaxes", "zaxis", "euler", "mass", "ipos", "iquat", "inertia", "iaxisangle", "ixyaxes", "izaxis", "ieuler", "fullinertia", "mocap", "gravcomp", "userdata", "explicitinertial", "plugin", "info"};

        py::dict kwarg_dict = kwargs;
        for (auto item: kwarg_dict) {
          std::string key = py::str(item.first);
          if (valid_kwargs.count(key) == 0) {
            throw pybind11::type_error("Invalid "
              + key
              + " keyword argument. Valid options are: name, childclass, pos, quat, axisangle, xyaxes, zaxis, euler, mass, ipos, iquat, inertia, iaxisangle, ixyaxes, izaxis, ieuler, fullinertia, mocap, gravcomp, userdata, explicitinertial, plugin, info.");
          }
        }
    

          auto set_string = [&kwargs](const char* str, std::basic_string<char>* des) {
            if (kwargs.contains(str)) {
              try {
                *des = kwargs[str].cast<std::string>();
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " should be a string.");
              }
            }
          };
          

          auto set_orientation = [&kwargs](raw::MjsOrientation& orientation,
            const char* axisangle,
            const char* xyaxes,
            const char* zaxis,
            const char* euler) {
            int nrepresentation = 0;
            bool has_axisangle = kwargs.contains(axisangle);
            nrepresentation += has_axisangle;
            bool has_xyaxes = kwargs.contains(xyaxes);
            nrepresentation += has_xyaxes;
            bool has_zaxis = kwargs.contains(zaxis);
            nrepresentation += has_zaxis;
            bool has_euler = kwargs.contains(euler);
            nrepresentation += has_euler;

            if (nrepresentation == 0) {
              return;
            } else if (nrepresentation > 1) {
              throw pybind11::value_error("Only one of: "
                + std::string(axisangle) + ", "
                + std::string(xyaxes) + ", "
                + std::string(zaxis)
                + ", or"
                + std::string(euler)
                + " can be set.");
            }

            auto set_array = [&kwargs](const char* str, double* des, int size) {
              try {
                std::vector<double> array = kwargs[str].cast<std::vector<double>>();
                if (array.size() != size) {
                  throw pybind11::value_error(std::string(str)
                    + " should be a list/array of size "
                    + std::to_string(size)
                    + ".");
                }
                int idx = 0;
                for (auto val : array) {
                  des[idx++] = val;
                }
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str)
                  + " should be a list/array.");
              }
            };

            if (has_axisangle) {
              set_array(axisangle, orientation.axisangle, 4);
              orientation.type = mjORIENTATION_AXISANGLE;
            } else if (has_xyaxes) {
              set_array(xyaxes, orientation.xyaxes, 6);
              orientation.type = mjORIENTATION_XYAXES;
            } else if (has_zaxis) {
              set_array(zaxis, orientation.zaxis, 3);
              orientation.type = mjORIENTATION_ZAXIS;
            } else if (has_euler) {
              set_array(euler, orientation.euler, 3);
              orientation.type = mjORIENTATION_EULER;
            }
          };
          

          auto set_vec = [&kwargs](const char* str, auto&& des) {
            if (kwargs.contains(str)) {
              try {
                using T = typename std::decay_t<decltype(*des)>::value_type;
                std::vector<T> vec = kwargs[str].cast<std::vector<T>>();
                des->clear();
                des->reserve(vec.size());
                for (auto val : vec) {
                  des->push_back(val);
                }
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " has the wrong type.");
              }
            }
          };
          

          auto set_value = [&kwargs](const char* str, auto&& des) {
            if (kwargs.contains(str)) {
              try {
                using T = std::decay_t<decltype(des)>;
                des = kwargs[str].cast<T>();
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " is the wrong type.");
              }
            }
          };
          

          auto set_array = [&kwargs](const char* str, auto&& des, int size) {
            if (kwargs.contains(str)) {
              try {
                using T = std::remove_pointer_t<std::decay_t<decltype(des)>>;
                std::vector<T> array = kwargs[str].cast<std::vector<T>>();
                if (array.size() != size) {
                  throw pybind11::value_error(std::string(str)
                    + " should be a list/array of size "
                    + std::to_string(size)
                    + ".");
                }
                int idx = 0;
                for (auto val : array) {
                  des[idx++] = val;
                }
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " should be a list/array.");
              }
            }
          };
          

          auto set_plugin = [&kwargs](raw::MjsPlugin& plugin) {
            if (kwargs.contains("plugin")) {
              std::optional<raw::MjsPlugin> input = kwargs["plugin"].cast<raw::MjsPlugin>();
              if (input.has_value()) {
                try {
                  plugin.name = input->name;
                } catch (const py::cast_error &e) {
                  throw pybind11::value_error("plugin.name should be a string.");
                }
                try {
                  plugin.plugin_name = input->plugin_name;
                } catch (const py::cast_error &e) {
                  throw pybind11::value_error("plugin.instance_name should be a string.");
                }
                try {
                  plugin.active = input->active;
                } catch (const py::cast_error &e) {
                  throw pybind11::value_error("plugin.active should be an mjtByte.");
                }
                try {
                  plugin.info = input->info;
                } catch (const py::cast_error &e) {
                  throw pybind11::value_error("plugin.info should be a string.");
                }
              }
            }
          };
          
        set_string("name", out->name);
        set_string("childclass", out->childclass);
        set_array("pos", out->pos, 3);
        set_array("quat", out->quat, 4);
        set_orientation(out->alt, "axisangle", "xyaxes", "zaxis", "euler");
        set_value("mass", out->mass);
        set_array("ipos", out->ipos, 3);
        set_array("iquat", out->iquat, 4);
        set_array("inertia", out->inertia, 3);
        set_orientation(out->ialt, "iaxisangle", "ixyaxes", "izaxis", "ieuler");
        set_array("fullinertia", out->fullinertia, 6);
        set_value("mocap", out->mocap);
        set_value("gravcomp", out->gravcomp);
        set_vec("userdata", out->userdata);
        set_value("explicitinertial", out->explicitinertial);
        set_plugin(out->plugin);
        set_string("info", out->info);

        return out;
      },
      py::arg_v("default", nullptr),
      py::return_value_policy::reference_internal);
    

      mjSpec.def_property_readonly(
        "bodies",
        [](MjSpec& self) -> py::list {
          py::list list;
          raw::MjsElement* el = mjs_firstElement(self.ptr, mjOBJ_BODY);
          while (el) {
            list.append(mjs_asBody(el));
            el = mjs_nextElement(self.ptr, el);
          }
          return list;
        },
        py::return_value_policy::reference_internal);
    

          mjsBody.def("add_site", [](raw::MjsBody& self,
            raw::MjsDefault* default_, py::kwargs kwargs) -> raw::MjsSite* {
            auto out = mjs_addSite(&self, default_);
        
        std::set<std::string> valid_kwargs = {"name", "pos", "quat", "axisangle", "xyaxes", "zaxis", "euler", "fromto", "size", "type", "material", "group", "rgba", "userdata", "info"};

        py::dict kwarg_dict = kwargs;
        for (auto item: kwarg_dict) {
          std::string key = py::str(item.first);
          if (valid_kwargs.count(key) == 0) {
            throw pybind11::type_error("Invalid "
              + key
              + " keyword argument. Valid options are: name, pos, quat, axisangle, xyaxes, zaxis, euler, fromto, size, type, material, group, rgba, userdata, info.");
          }
        }
    

          auto set_string = [&kwargs](const char* str, std::basic_string<char>* des) {
            if (kwargs.contains(str)) {
              try {
                *des = kwargs[str].cast<std::string>();
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " should be a string.");
              }
            }
          };
          

          auto set_orientation = [&kwargs](raw::MjsOrientation& orientation,
            const char* axisangle,
            const char* xyaxes,
            const char* zaxis,
            const char* euler) {
            int nrepresentation = 0;
            bool has_axisangle = kwargs.contains(axisangle);
            nrepresentation += has_axisangle;
            bool has_xyaxes = kwargs.contains(xyaxes);
            nrepresentation += has_xyaxes;
            bool has_zaxis = kwargs.contains(zaxis);
            nrepresentation += has_zaxis;
            bool has_euler = kwargs.contains(euler);
            nrepresentation += has_euler;

            if (nrepresentation == 0) {
              return;
            } else if (nrepresentation > 1) {
              throw pybind11::value_error("Only one of: "
                + std::string(axisangle) + ", "
                + std::string(xyaxes) + ", "
                + std::string(zaxis)
                + ", or"
                + std::string(euler)
                + " can be set.");
            }

            auto set_array = [&kwargs](const char* str, double* des, int size) {
              try {
                std::vector<double> array = kwargs[str].cast<std::vector<double>>();
                if (array.size() != size) {
                  throw pybind11::value_error(std::string(str)
                    + " should be a list/array of size "
                    + std::to_string(size)
                    + ".");
                }
                int idx = 0;
                for (auto val : array) {
                  des[idx++] = val;
                }
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str)
                  + " should be a list/array.");
              }
            };

            if (has_axisangle) {
              set_array(axisangle, orientation.axisangle, 4);
              orientation.type = mjORIENTATION_AXISANGLE;
            } else if (has_xyaxes) {
              set_array(xyaxes, orientation.xyaxes, 6);
              orientation.type = mjORIENTATION_XYAXES;
            } else if (has_zaxis) {
              set_array(zaxis, orientation.zaxis, 3);
              orientation.type = mjORIENTATION_ZAXIS;
            } else if (has_euler) {
              set_array(euler, orientation.euler, 3);
              orientation.type = mjORIENTATION_EULER;
            }
          };
          

          auto set_vec = [&kwargs](const char* str, auto&& des) {
            if (kwargs.contains(str)) {
              try {
                using T = typename std::decay_t<decltype(*des)>::value_type;
                std::vector<T> vec = kwargs[str].cast<std::vector<T>>();
                des->clear();
                des->reserve(vec.size());
                for (auto val : vec) {
                  des->push_back(val);
                }
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " has the wrong type.");
              }
            }
          };
          

          auto set_value = [&kwargs](const char* str, auto&& des) {
            if (kwargs.contains(str)) {
              try {
                using T = std::decay_t<decltype(des)>;
                des = kwargs[str].cast<T>();
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " is the wrong type.");
              }
            }
          };
          

          auto set_array = [&kwargs](const char* str, auto&& des, int size) {
            if (kwargs.contains(str)) {
              try {
                using T = std::remove_pointer_t<std::decay_t<decltype(des)>>;
                std::vector<T> array = kwargs[str].cast<std::vector<T>>();
                if (array.size() != size) {
                  throw pybind11::value_error(std::string(str)
                    + " should be a list/array of size "
                    + std::to_string(size)
                    + ".");
                }
                int idx = 0;
                for (auto val : array) {
                  des[idx++] = val;
                }
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " should be a list/array.");
              }
            }
          };
          
        set_string("name", out->name);
        set_array("pos", out->pos, 3);
        set_array("quat", out->quat, 4);
        set_orientation(out->alt, "axisangle", "xyaxes", "zaxis", "euler");
        set_array("fromto", out->fromto, 6);
        set_array("size", out->size, 3);
        set_value("type", out->type);
        set_string("material", out->material);
        set_value("group", out->group);
        set_array("rgba", out->rgba, 4);
        set_vec("userdata", out->userdata);
        set_string("info", out->info);

        return out;
      },
      py::arg_v("default", nullptr),
      py::return_value_policy::reference_internal);
    

      mjSpec.def_property_readonly(
        "sites",
        [](MjSpec& self) -> py::list {
          py::list list;
          raw::MjsElement* el = mjs_firstElement(self.ptr, mjOBJ_SITE);
          while (el) {
            list.append(mjs_asSite(el));
            el = mjs_nextElement(self.ptr, el);
          }
          return list;
        },
        py::return_value_policy::reference_internal);
    

          mjsBody.def("add_geom", [](raw::MjsBody& self,
            raw::MjsDefault* default_, py::kwargs kwargs) -> raw::MjsGeom* {
            auto out = mjs_addGeom(&self, default_);
        
        std::set<std::string> valid_kwargs = {"name", "type", "pos", "quat", "axisangle", "xyaxes", "zaxis", "euler", "fromto", "size", "contype", "conaffinity", "condim", "priority", "friction", "solmix", "solref", "solimp", "margin", "gap", "mass", "density", "typeinertia", "fluid_ellipsoid", "fluid_coefs", "material", "rgba", "group", "hfieldname", "meshname", "fitscale", "userdata", "plugin", "info"};

        py::dict kwarg_dict = kwargs;
        for (auto item: kwarg_dict) {
          std::string key = py::str(item.first);
          if (valid_kwargs.count(key) == 0) {
            throw pybind11::type_error("Invalid "
              + key
              + " keyword argument. Valid options are: name, type, pos, quat, axisangle, xyaxes, zaxis, euler, fromto, size, contype, conaffinity, condim, priority, friction, solmix, solref, solimp, margin, gap, mass, density, typeinertia, fluid_ellipsoid, fluid_coefs, material, rgba, group, hfieldname, meshname, fitscale, userdata, plugin, info.");
          }
        }
    

          auto set_string = [&kwargs](const char* str, std::basic_string<char>* des) {
            if (kwargs.contains(str)) {
              try {
                *des = kwargs[str].cast<std::string>();
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " should be a string.");
              }
            }
          };
          

          auto set_orientation = [&kwargs](raw::MjsOrientation& orientation,
            const char* axisangle,
            const char* xyaxes,
            const char* zaxis,
            const char* euler) {
            int nrepresentation = 0;
            bool has_axisangle = kwargs.contains(axisangle);
            nrepresentation += has_axisangle;
            bool has_xyaxes = kwargs.contains(xyaxes);
            nrepresentation += has_xyaxes;
            bool has_zaxis = kwargs.contains(zaxis);
            nrepresentation += has_zaxis;
            bool has_euler = kwargs.contains(euler);
            nrepresentation += has_euler;

            if (nrepresentation == 0) {
              return;
            } else if (nrepresentation > 1) {
              throw pybind11::value_error("Only one of: "
                + std::string(axisangle) + ", "
                + std::string(xyaxes) + ", "
                + std::string(zaxis)
                + ", or"
                + std::string(euler)
                + " can be set.");
            }

            auto set_array = [&kwargs](const char* str, double* des, int size) {
              try {
                std::vector<double> array = kwargs[str].cast<std::vector<double>>();
                if (array.size() != size) {
                  throw pybind11::value_error(std::string(str)
                    + " should be a list/array of size "
                    + std::to_string(size)
                    + ".");
                }
                int idx = 0;
                for (auto val : array) {
                  des[idx++] = val;
                }
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str)
                  + " should be a list/array.");
              }
            };

            if (has_axisangle) {
              set_array(axisangle, orientation.axisangle, 4);
              orientation.type = mjORIENTATION_AXISANGLE;
            } else if (has_xyaxes) {
              set_array(xyaxes, orientation.xyaxes, 6);
              orientation.type = mjORIENTATION_XYAXES;
            } else if (has_zaxis) {
              set_array(zaxis, orientation.zaxis, 3);
              orientation.type = mjORIENTATION_ZAXIS;
            } else if (has_euler) {
              set_array(euler, orientation.euler, 3);
              orientation.type = mjORIENTATION_EULER;
            }
          };
          

          auto set_vec = [&kwargs](const char* str, auto&& des) {
            if (kwargs.contains(str)) {
              try {
                using T = typename std::decay_t<decltype(*des)>::value_type;
                std::vector<T> vec = kwargs[str].cast<std::vector<T>>();
                des->clear();
                des->reserve(vec.size());
                for (auto val : vec) {
                  des->push_back(val);
                }
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " has the wrong type.");
              }
            }
          };
          

          auto set_value = [&kwargs](const char* str, auto&& des) {
            if (kwargs.contains(str)) {
              try {
                using T = std::decay_t<decltype(des)>;
                des = kwargs[str].cast<T>();
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " is the wrong type.");
              }
            }
          };
          

          auto set_array = [&kwargs](const char* str, auto&& des, int size) {
            if (kwargs.contains(str)) {
              try {
                using T = std::remove_pointer_t<std::decay_t<decltype(des)>>;
                std::vector<T> array = kwargs[str].cast<std::vector<T>>();
                if (array.size() != size) {
                  throw pybind11::value_error(std::string(str)
                    + " should be a list/array of size "
                    + std::to_string(size)
                    + ".");
                }
                int idx = 0;
                for (auto val : array) {
                  des[idx++] = val;
                }
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " should be a list/array.");
              }
            }
          };
          

          auto set_plugin = [&kwargs](raw::MjsPlugin& plugin) {
            if (kwargs.contains("plugin")) {
              std::optional<raw::MjsPlugin> input = kwargs["plugin"].cast<raw::MjsPlugin>();
              if (input.has_value()) {
                try {
                  plugin.name = input->name;
                } catch (const py::cast_error &e) {
                  throw pybind11::value_error("plugin.name should be a string.");
                }
                try {
                  plugin.plugin_name = input->plugin_name;
                } catch (const py::cast_error &e) {
                  throw pybind11::value_error("plugin.instance_name should be a string.");
                }
                try {
                  plugin.active = input->active;
                } catch (const py::cast_error &e) {
                  throw pybind11::value_error("plugin.active should be an mjtByte.");
                }
                try {
                  plugin.info = input->info;
                } catch (const py::cast_error &e) {
                  throw pybind11::value_error("plugin.info should be a string.");
                }
              }
            }
          };
          
        set_string("name", out->name);
        set_value("type", out->type);
        set_array("pos", out->pos, 3);
        set_array("quat", out->quat, 4);
        set_orientation(out->alt, "axisangle", "xyaxes", "zaxis", "euler");
        set_array("fromto", out->fromto, 6);
        set_array("size", out->size, 3);
        set_value("contype", out->contype);
        set_value("conaffinity", out->conaffinity);
        set_value("condim", out->condim);
        set_value("priority", out->priority);
        set_array("friction", out->friction, 3);
        set_value("solmix", out->solmix);
        set_array("solref", out->solref, 2);
        set_array("solimp", out->solimp, 5);
        set_value("margin", out->margin);
        set_value("gap", out->gap);
        set_value("mass", out->mass);
        set_value("density", out->density);
        set_value("typeinertia", out->typeinertia);
        set_value("fluid_ellipsoid", out->fluid_ellipsoid);
        set_array("fluid_coefs", out->fluid_coefs, 5);
        set_string("material", out->material);
        set_array("rgba", out->rgba, 4);
        set_value("group", out->group);
        set_string("hfieldname", out->hfieldname);
        set_string("meshname", out->meshname);
        set_value("fitscale", out->fitscale);
        set_vec("userdata", out->userdata);
        set_plugin(out->plugin);
        set_string("info", out->info);

        return out;
      },
      py::arg_v("default", nullptr),
      py::return_value_policy::reference_internal);
    

      mjSpec.def_property_readonly(
        "geoms",
        [](MjSpec& self) -> py::list {
          py::list list;
          raw::MjsElement* el = mjs_firstElement(self.ptr, mjOBJ_GEOM);
          while (el) {
            list.append(mjs_asGeom(el));
            el = mjs_nextElement(self.ptr, el);
          }
          return list;
        },
        py::return_value_policy::reference_internal);
    

          mjsBody.def("add_joint", [](raw::MjsBody& self,
            raw::MjsDefault* default_, py::kwargs kwargs) -> raw::MjsJoint* {
            auto out = mjs_addJoint(&self, default_);
        
        std::set<std::string> valid_kwargs = {"name", "type", "pos", "axis", "ref", "align", "stiffness", "springref", "springdamper", "limited", "range", "margin", "solref_limit", "solimp_limit", "actfrclimited", "actfrcrange", "armature", "damping", "frictionloss", "solref_friction", "solimp_friction", "group", "actgravcomp", "userdata", "info"};

        py::dict kwarg_dict = kwargs;
        for (auto item: kwarg_dict) {
          std::string key = py::str(item.first);
          if (valid_kwargs.count(key) == 0) {
            throw pybind11::type_error("Invalid "
              + key
              + " keyword argument. Valid options are: name, type, pos, axis, ref, align, stiffness, springref, springdamper, limited, range, margin, solref_limit, solimp_limit, actfrclimited, actfrcrange, armature, damping, frictionloss, solref_friction, solimp_friction, group, actgravcomp, userdata, info.");
          }
        }
    

          auto set_vec = [&kwargs](const char* str, auto&& des) {
            if (kwargs.contains(str)) {
              try {
                using T = typename std::decay_t<decltype(*des)>::value_type;
                std::vector<T> vec = kwargs[str].cast<std::vector<T>>();
                des->clear();
                des->reserve(vec.size());
                for (auto val : vec) {
                  des->push_back(val);
                }
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " has the wrong type.");
              }
            }
          };
          

          auto set_array = [&kwargs](const char* str, auto&& des, int size) {
            if (kwargs.contains(str)) {
              try {
                using T = std::remove_pointer_t<std::decay_t<decltype(des)>>;
                std::vector<T> array = kwargs[str].cast<std::vector<T>>();
                if (array.size() != size) {
                  throw pybind11::value_error(std::string(str)
                    + " should be a list/array of size "
                    + std::to_string(size)
                    + ".");
                }
                int idx = 0;
                for (auto val : array) {
                  des[idx++] = val;
                }
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " should be a list/array.");
              }
            }
          };
          

          auto set_string = [&kwargs](const char* str, std::basic_string<char>* des) {
            if (kwargs.contains(str)) {
              try {
                *des = kwargs[str].cast<std::string>();
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " should be a string.");
              }
            }
          };
          

          auto set_value = [&kwargs](const char* str, auto&& des) {
            if (kwargs.contains(str)) {
              try {
                using T = std::decay_t<decltype(des)>;
                des = kwargs[str].cast<T>();
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " is the wrong type.");
              }
            }
          };
          
        set_string("name", out->name);
        set_value("type", out->type);
        set_array("pos", out->pos, 3);
        set_array("axis", out->axis, 3);
        set_value("ref", out->ref);
        set_value("align", out->align);
        set_value("stiffness", out->stiffness);
        set_value("springref", out->springref);
        set_array("springdamper", out->springdamper, 2);
        set_value("limited", out->limited);
        set_array("range", out->range, 2);
        set_value("margin", out->margin);
        set_array("solref_limit", out->solref_limit, 2);
        set_array("solimp_limit", out->solimp_limit, 5);
        set_value("actfrclimited", out->actfrclimited);
        set_array("actfrcrange", out->actfrcrange, 2);
        set_value("armature", out->armature);
        set_value("damping", out->damping);
        set_value("frictionloss", out->frictionloss);
        set_array("solref_friction", out->solref_friction, 2);
        set_array("solimp_friction", out->solimp_friction, 5);
        set_value("group", out->group);
        set_value("actgravcomp", out->actgravcomp);
        set_vec("userdata", out->userdata);
        set_string("info", out->info);

        return out;
      },
      py::arg_v("default", nullptr),
      py::return_value_policy::reference_internal);
    

      mjSpec.def_property_readonly(
        "joints",
        [](MjSpec& self) -> py::list {
          py::list list;
          raw::MjsElement* el = mjs_firstElement(self.ptr, mjOBJ_JOINT);
          while (el) {
            list.append(mjs_asJoint(el));
            el = mjs_nextElement(self.ptr, el);
          }
          return list;
        },
        py::return_value_policy::reference_internal);
    

          mjsBody.def("add_camera", [](raw::MjsBody& self,
            raw::MjsDefault* default_, py::kwargs kwargs) -> raw::MjsCamera* {
            auto out = mjs_addCamera(&self, default_);
        
        std::set<std::string> valid_kwargs = {"name", "pos", "quat", "axisangle", "xyaxes", "zaxis", "euler", "mode", "targetbody", "orthographic", "fovy", "ipd", "intrinsic", "sensor_size", "resolution", "focal_length", "focal_pixel", "principal_length", "principal_pixel", "userdata", "info"};

        py::dict kwarg_dict = kwargs;
        for (auto item: kwarg_dict) {
          std::string key = py::str(item.first);
          if (valid_kwargs.count(key) == 0) {
            throw pybind11::type_error("Invalid "
              + key
              + " keyword argument. Valid options are: name, pos, quat, axisangle, xyaxes, zaxis, euler, mode, targetbody, orthographic, fovy, ipd, intrinsic, sensor_size, resolution, focal_length, focal_pixel, principal_length, principal_pixel, userdata, info.");
          }
        }
    

          auto set_string = [&kwargs](const char* str, std::basic_string<char>* des) {
            if (kwargs.contains(str)) {
              try {
                *des = kwargs[str].cast<std::string>();
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " should be a string.");
              }
            }
          };
          

          auto set_orientation = [&kwargs](raw::MjsOrientation& orientation,
            const char* axisangle,
            const char* xyaxes,
            const char* zaxis,
            const char* euler) {
            int nrepresentation = 0;
            bool has_axisangle = kwargs.contains(axisangle);
            nrepresentation += has_axisangle;
            bool has_xyaxes = kwargs.contains(xyaxes);
            nrepresentation += has_xyaxes;
            bool has_zaxis = kwargs.contains(zaxis);
            nrepresentation += has_zaxis;
            bool has_euler = kwargs.contains(euler);
            nrepresentation += has_euler;

            if (nrepresentation == 0) {
              return;
            } else if (nrepresentation > 1) {
              throw pybind11::value_error("Only one of: "
                + std::string(axisangle) + ", "
                + std::string(xyaxes) + ", "
                + std::string(zaxis)
                + ", or"
                + std::string(euler)
                + " can be set.");
            }

            auto set_array = [&kwargs](const char* str, double* des, int size) {
              try {
                std::vector<double> array = kwargs[str].cast<std::vector<double>>();
                if (array.size() != size) {
                  throw pybind11::value_error(std::string(str)
                    + " should be a list/array of size "
                    + std::to_string(size)
                    + ".");
                }
                int idx = 0;
                for (auto val : array) {
                  des[idx++] = val;
                }
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str)
                  + " should be a list/array.");
              }
            };

            if (has_axisangle) {
              set_array(axisangle, orientation.axisangle, 4);
              orientation.type = mjORIENTATION_AXISANGLE;
            } else if (has_xyaxes) {
              set_array(xyaxes, orientation.xyaxes, 6);
              orientation.type = mjORIENTATION_XYAXES;
            } else if (has_zaxis) {
              set_array(zaxis, orientation.zaxis, 3);
              orientation.type = mjORIENTATION_ZAXIS;
            } else if (has_euler) {
              set_array(euler, orientation.euler, 3);
              orientation.type = mjORIENTATION_EULER;
            }
          };
          

          auto set_vec = [&kwargs](const char* str, auto&& des) {
            if (kwargs.contains(str)) {
              try {
                using T = typename std::decay_t<decltype(*des)>::value_type;
                std::vector<T> vec = kwargs[str].cast<std::vector<T>>();
                des->clear();
                des->reserve(vec.size());
                for (auto val : vec) {
                  des->push_back(val);
                }
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " has the wrong type.");
              }
            }
          };
          

          auto set_value = [&kwargs](const char* str, auto&& des) {
            if (kwargs.contains(str)) {
              try {
                using T = std::decay_t<decltype(des)>;
                des = kwargs[str].cast<T>();
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " is the wrong type.");
              }
            }
          };
          

          auto set_array = [&kwargs](const char* str, auto&& des, int size) {
            if (kwargs.contains(str)) {
              try {
                using T = std::remove_pointer_t<std::decay_t<decltype(des)>>;
                std::vector<T> array = kwargs[str].cast<std::vector<T>>();
                if (array.size() != size) {
                  throw pybind11::value_error(std::string(str)
                    + " should be a list/array of size "
                    + std::to_string(size)
                    + ".");
                }
                int idx = 0;
                for (auto val : array) {
                  des[idx++] = val;
                }
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " should be a list/array.");
              }
            }
          };
          
        set_string("name", out->name);
        set_array("pos", out->pos, 3);
        set_array("quat", out->quat, 4);
        set_orientation(out->alt, "axisangle", "xyaxes", "zaxis", "euler");
        set_value("mode", out->mode);
        set_string("targetbody", out->targetbody);
        set_value("orthographic", out->orthographic);
        set_value("fovy", out->fovy);
        set_value("ipd", out->ipd);
        set_array("intrinsic", out->intrinsic, 4);
        set_array("sensor_size", out->sensor_size, 2);
        set_array("resolution", out->resolution, 2);
        set_array("focal_length", out->focal_length, 2);
        set_array("focal_pixel", out->focal_pixel, 2);
        set_array("principal_length", out->principal_length, 2);
        set_array("principal_pixel", out->principal_pixel, 2);
        set_vec("userdata", out->userdata);
        set_string("info", out->info);

        return out;
      },
      py::arg_v("default", nullptr),
      py::return_value_policy::reference_internal);
    

      mjSpec.def_property_readonly(
        "cameras",
        [](MjSpec& self) -> py::list {
          py::list list;
          raw::MjsElement* el = mjs_firstElement(self.ptr, mjOBJ_CAMERA);
          while (el) {
            list.append(mjs_asCamera(el));
            el = mjs_nextElement(self.ptr, el);
          }
          return list;
        },
        py::return_value_policy::reference_internal);
    

          mjsBody.def("add_frame", [](raw::MjsBody& self,
            raw::MjsFrame* parentframe_, py::kwargs kwargs) -> raw::MjsFrame* {
            auto out = mjs_addFrame(&self, parentframe_);
        
        std::set<std::string> valid_kwargs = {"name", "childclass", "pos", "quat", "axisangle", "xyaxes", "zaxis", "euler", "info"};

        py::dict kwarg_dict = kwargs;
        for (auto item: kwarg_dict) {
          std::string key = py::str(item.first);
          if (valid_kwargs.count(key) == 0) {
            throw pybind11::type_error("Invalid "
              + key
              + " keyword argument. Valid options are: name, childclass, pos, quat, axisangle, xyaxes, zaxis, euler, info.");
          }
        }
    

          auto set_array = [&kwargs](const char* str, auto&& des, int size) {
            if (kwargs.contains(str)) {
              try {
                using T = std::remove_pointer_t<std::decay_t<decltype(des)>>;
                std::vector<T> array = kwargs[str].cast<std::vector<T>>();
                if (array.size() != size) {
                  throw pybind11::value_error(std::string(str)
                    + " should be a list/array of size "
                    + std::to_string(size)
                    + ".");
                }
                int idx = 0;
                for (auto val : array) {
                  des[idx++] = val;
                }
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " should be a list/array.");
              }
            }
          };
          

          auto set_string = [&kwargs](const char* str, std::basic_string<char>* des) {
            if (kwargs.contains(str)) {
              try {
                *des = kwargs[str].cast<std::string>();
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " should be a string.");
              }
            }
          };
          

          auto set_orientation = [&kwargs](raw::MjsOrientation& orientation,
            const char* axisangle,
            const char* xyaxes,
            const char* zaxis,
            const char* euler) {
            int nrepresentation = 0;
            bool has_axisangle = kwargs.contains(axisangle);
            nrepresentation += has_axisangle;
            bool has_xyaxes = kwargs.contains(xyaxes);
            nrepresentation += has_xyaxes;
            bool has_zaxis = kwargs.contains(zaxis);
            nrepresentation += has_zaxis;
            bool has_euler = kwargs.contains(euler);
            nrepresentation += has_euler;

            if (nrepresentation == 0) {
              return;
            } else if (nrepresentation > 1) {
              throw pybind11::value_error("Only one of: "
                + std::string(axisangle) + ", "
                + std::string(xyaxes) + ", "
                + std::string(zaxis)
                + ", or"
                + std::string(euler)
                + " can be set.");
            }

            auto set_array = [&kwargs](const char* str, double* des, int size) {
              try {
                std::vector<double> array = kwargs[str].cast<std::vector<double>>();
                if (array.size() != size) {
                  throw pybind11::value_error(std::string(str)
                    + " should be a list/array of size "
                    + std::to_string(size)
                    + ".");
                }
                int idx = 0;
                for (auto val : array) {
                  des[idx++] = val;
                }
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str)
                  + " should be a list/array.");
              }
            };

            if (has_axisangle) {
              set_array(axisangle, orientation.axisangle, 4);
              orientation.type = mjORIENTATION_AXISANGLE;
            } else if (has_xyaxes) {
              set_array(xyaxes, orientation.xyaxes, 6);
              orientation.type = mjORIENTATION_XYAXES;
            } else if (has_zaxis) {
              set_array(zaxis, orientation.zaxis, 3);
              orientation.type = mjORIENTATION_ZAXIS;
            } else if (has_euler) {
              set_array(euler, orientation.euler, 3);
              orientation.type = mjORIENTATION_EULER;
            }
          };
          
        set_string("name", out->name);
        set_string("childclass", out->childclass);
        set_array("pos", out->pos, 3);
        set_array("quat", out->quat, 4);
        set_orientation(out->alt, "axisangle", "xyaxes", "zaxis", "euler");
        set_string("info", out->info);

        return out;
      },
      py::arg_v("default", nullptr),
      py::return_value_policy::reference_internal);
    

      mjSpec.def_property_readonly(
        "frames",
        [](MjSpec& self) -> py::list {
          py::list list;
          raw::MjsElement* el = mjs_firstElement(self.ptr, mjOBJ_FRAME);
          while (el) {
            list.append(mjs_asFrame(el));
            el = mjs_nextElement(self.ptr, el);
          }
          return list;
        },
        py::return_value_policy::reference_internal);
    

          mjsBody.def("add_light", [](raw::MjsBody& self,
            raw::MjsDefault* default_, py::kwargs kwargs) -> raw::MjsLight* {
            auto out = mjs_addLight(&self, default_);
        
        std::set<std::string> valid_kwargs = {"name", "pos", "dir", "mode", "targetbody", "active", "directional", "castshadow", "bulbradius", "attenuation", "cutoff", "exponent", "ambient", "diffuse", "specular", "info"};

        py::dict kwarg_dict = kwargs;
        for (auto item: kwarg_dict) {
          std::string key = py::str(item.first);
          if (valid_kwargs.count(key) == 0) {
            throw pybind11::type_error("Invalid "
              + key
              + " keyword argument. Valid options are: name, pos, dir, mode, targetbody, active, directional, castshadow, bulbradius, attenuation, cutoff, exponent, ambient, diffuse, specular, info.");
          }
        }
    

          auto set_array = [&kwargs](const char* str, auto&& des, int size) {
            if (kwargs.contains(str)) {
              try {
                using T = std::remove_pointer_t<std::decay_t<decltype(des)>>;
                std::vector<T> array = kwargs[str].cast<std::vector<T>>();
                if (array.size() != size) {
                  throw pybind11::value_error(std::string(str)
                    + " should be a list/array of size "
                    + std::to_string(size)
                    + ".");
                }
                int idx = 0;
                for (auto val : array) {
                  des[idx++] = val;
                }
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " should be a list/array.");
              }
            }
          };
          

          auto set_string = [&kwargs](const char* str, std::basic_string<char>* des) {
            if (kwargs.contains(str)) {
              try {
                *des = kwargs[str].cast<std::string>();
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " should be a string.");
              }
            }
          };
          

          auto set_value = [&kwargs](const char* str, auto&& des) {
            if (kwargs.contains(str)) {
              try {
                using T = std::decay_t<decltype(des)>;
                des = kwargs[str].cast<T>();
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " is the wrong type.");
              }
            }
          };
          
        set_string("name", out->name);
        set_array("pos", out->pos, 3);
        set_array("dir", out->dir, 3);
        set_value("mode", out->mode);
        set_string("targetbody", out->targetbody);
        set_value("active", out->active);
        set_value("directional", out->directional);
        set_value("castshadow", out->castshadow);
        set_value("bulbradius", out->bulbradius);
        set_array("attenuation", out->attenuation, 3);
        set_value("cutoff", out->cutoff);
        set_value("exponent", out->exponent);
        set_array("ambient", out->ambient, 3);
        set_array("diffuse", out->diffuse, 3);
        set_array("specular", out->specular, 3);
        set_string("info", out->info);

        return out;
      },
      py::arg_v("default", nullptr),
      py::return_value_policy::reference_internal);
    

      mjSpec.def_property_readonly(
        "lights",
        [](MjSpec& self) -> py::list {
          py::list list;
          raw::MjsElement* el = mjs_firstElement(self.ptr, mjOBJ_LIGHT);
          while (el) {
            list.append(mjs_asLight(el));
            el = mjs_nextElement(self.ptr, el);
          }
          return list;
        },
        py::return_value_policy::reference_internal);
    

          mjSpec.def("add_flex", [](MjSpec& self, py::kwargs kwargs) -> raw::MjsFlex* {
            auto out = mjs_addFlex(self.ptr);
        
        std::set<std::string> valid_kwargs = {"name", "contype", "conaffinity", "condim", "priority", "friction", "solmix", "solref", "solimp", "margin", "gap", "dim", "radius", "internal", "flatskin", "selfcollide", "activelayers", "group", "edgestiffness", "edgedamping", "rgba", "material", "young", "poisson", "damping", "thickness", "nodebody", "vertbody", "node", "vert", "elem", "texcoord", "info"};

        py::dict kwarg_dict = kwargs;
        for (auto item: kwarg_dict) {
          std::string key = py::str(item.first);
          if (valid_kwargs.count(key) == 0) {
            throw pybind11::type_error("Invalid "
              + key
              + " keyword argument. Valid options are: name, contype, conaffinity, condim, priority, friction, solmix, solref, solimp, margin, gap, dim, radius, internal, flatskin, selfcollide, activelayers, group, edgestiffness, edgedamping, rgba, material, young, poisson, damping, thickness, nodebody, vertbody, node, vert, elem, texcoord, info.");
          }
        }
    

          auto set_vec = [&kwargs](const char* str, auto&& des) {
            if (kwargs.contains(str)) {
              try {
                using T = typename std::decay_t<decltype(*des)>::value_type;
                std::vector<T> vec = kwargs[str].cast<std::vector<T>>();
                des->clear();
                des->reserve(vec.size());
                for (auto val : vec) {
                  des->push_back(val);
                }
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " has the wrong type.");
              }
            }
          };
          

          auto set_array = [&kwargs](const char* str, auto&& des, int size) {
            if (kwargs.contains(str)) {
              try {
                using T = std::remove_pointer_t<std::decay_t<decltype(des)>>;
                std::vector<T> array = kwargs[str].cast<std::vector<T>>();
                if (array.size() != size) {
                  throw pybind11::value_error(std::string(str)
                    + " should be a list/array of size "
                    + std::to_string(size)
                    + ".");
                }
                int idx = 0;
                for (auto val : array) {
                  des[idx++] = val;
                }
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " should be a list/array.");
              }
            }
          };
          

          auto set_string = [&kwargs](const char* str, std::basic_string<char>* des) {
            if (kwargs.contains(str)) {
              try {
                *des = kwargs[str].cast<std::string>();
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " should be a string.");
              }
            }
          };
          

          auto set_value = [&kwargs](const char* str, auto&& des) {
            if (kwargs.contains(str)) {
              try {
                using T = std::decay_t<decltype(des)>;
                des = kwargs[str].cast<T>();
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " is the wrong type.");
              }
            }
          };
          
        set_string("name", out->name);
        set_value("contype", out->contype);
        set_value("conaffinity", out->conaffinity);
        set_value("condim", out->condim);
        set_value("priority", out->priority);
        set_array("friction", out->friction, 3);
        set_value("solmix", out->solmix);
        set_array("solref", out->solref, 2);
        set_array("solimp", out->solimp, 5);
        set_value("margin", out->margin);
        set_value("gap", out->gap);
        set_value("dim", out->dim);
        set_value("radius", out->radius);
        set_value("internal", out->internal);
        set_value("flatskin", out->flatskin);
        set_value("selfcollide", out->selfcollide);
        set_value("activelayers", out->activelayers);
        set_value("group", out->group);
        set_value("edgestiffness", out->edgestiffness);
        set_value("edgedamping", out->edgedamping);
        set_array("rgba", out->rgba, 4);
        set_string("material", out->material);
        set_value("young", out->young);
        set_value("poisson", out->poisson);
        set_value("damping", out->damping);
        set_value("thickness", out->thickness);
        set_vec("nodebody", out->nodebody);
        set_vec("vertbody", out->vertbody);
        set_vec("node", out->node);
        set_vec("vert", out->vert);
        set_vec("elem", out->elem);
        set_vec("texcoord", out->texcoord);
        set_string("info", out->info);

        return out;
      },
      
      py::return_value_policy::reference_internal);
    

      mjSpec.def_property_readonly(
        "flexes",
        [](MjSpec& self) -> py::list {
          py::list list;
          raw::MjsElement* el = mjs_firstElement(self.ptr, mjOBJ_FLEX);
          while (el) {
            list.append(mjs_asFlex(el));
            el = mjs_nextElement(self.ptr, el);
          }
          return list;
        },
        py::return_value_policy::reference_internal);
    

          mjSpec.def("add_mesh", [](MjSpec& self,
            raw::MjsDefault* default_, py::kwargs kwargs) -> raw::MjsMesh* {
            auto out = mjs_addMesh(self.ptr, default_);
        
        std::set<std::string> valid_kwargs = {"name", "content_type", "file", "refpos", "refquat", "scale", "inertia", "smoothnormal", "maxhullvert", "uservert", "usernormal", "usertexcoord", "userface", "userfacetexcoord", "plugin", "info"};

        py::dict kwarg_dict = kwargs;
        for (auto item: kwarg_dict) {
          std::string key = py::str(item.first);
          if (valid_kwargs.count(key) == 0) {
            throw pybind11::type_error("Invalid "
              + key
              + " keyword argument. Valid options are: name, content_type, file, refpos, refquat, scale, inertia, smoothnormal, maxhullvert, uservert, usernormal, usertexcoord, userface, userfacetexcoord, plugin, info.");
          }
        }
    

          auto set_string = [&kwargs](const char* str, std::basic_string<char>* des) {
            if (kwargs.contains(str)) {
              try {
                *des = kwargs[str].cast<std::string>();
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " should be a string.");
              }
            }
          };
          

          auto set_vec = [&kwargs](const char* str, auto&& des) {
            if (kwargs.contains(str)) {
              try {
                using T = typename std::decay_t<decltype(*des)>::value_type;
                std::vector<T> vec = kwargs[str].cast<std::vector<T>>();
                des->clear();
                des->reserve(vec.size());
                for (auto val : vec) {
                  des->push_back(val);
                }
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " has the wrong type.");
              }
            }
          };
          

          auto set_value = [&kwargs](const char* str, auto&& des) {
            if (kwargs.contains(str)) {
              try {
                using T = std::decay_t<decltype(des)>;
                des = kwargs[str].cast<T>();
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " is the wrong type.");
              }
            }
          };
          

          auto set_array = [&kwargs](const char* str, auto&& des, int size) {
            if (kwargs.contains(str)) {
              try {
                using T = std::remove_pointer_t<std::decay_t<decltype(des)>>;
                std::vector<T> array = kwargs[str].cast<std::vector<T>>();
                if (array.size() != size) {
                  throw pybind11::value_error(std::string(str)
                    + " should be a list/array of size "
                    + std::to_string(size)
                    + ".");
                }
                int idx = 0;
                for (auto val : array) {
                  des[idx++] = val;
                }
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " should be a list/array.");
              }
            }
          };
          

          auto set_plugin = [&kwargs](raw::MjsPlugin& plugin) {
            if (kwargs.contains("plugin")) {
              std::optional<raw::MjsPlugin> input = kwargs["plugin"].cast<raw::MjsPlugin>();
              if (input.has_value()) {
                try {
                  plugin.name = input->name;
                } catch (const py::cast_error &e) {
                  throw pybind11::value_error("plugin.name should be a string.");
                }
                try {
                  plugin.plugin_name = input->plugin_name;
                } catch (const py::cast_error &e) {
                  throw pybind11::value_error("plugin.instance_name should be a string.");
                }
                try {
                  plugin.active = input->active;
                } catch (const py::cast_error &e) {
                  throw pybind11::value_error("plugin.active should be an mjtByte.");
                }
                try {
                  plugin.info = input->info;
                } catch (const py::cast_error &e) {
                  throw pybind11::value_error("plugin.info should be a string.");
                }
              }
            }
          };
          
        set_string("name", out->name);
        set_string("content_type", out->content_type);
        set_string("file", out->file);
        set_array("refpos", out->refpos, 3);
        set_array("refquat", out->refquat, 4);
        set_array("scale", out->scale, 3);
        set_value("inertia", out->inertia);
        set_value("smoothnormal", out->smoothnormal);
        set_value("maxhullvert", out->maxhullvert);
        set_vec("uservert", out->uservert);
        set_vec("usernormal", out->usernormal);
        set_vec("usertexcoord", out->usertexcoord);
        set_vec("userface", out->userface);
        set_vec("userfacetexcoord", out->userfacetexcoord);
        set_plugin(out->plugin);
        set_string("info", out->info);

        return out;
      },
      py::arg_v("default", nullptr),
      py::return_value_policy::reference_internal);
    

      mjSpec.def_property_readonly(
        "meshes",
        [](MjSpec& self) -> py::list {
          py::list list;
          raw::MjsElement* el = mjs_firstElement(self.ptr, mjOBJ_MESH);
          while (el) {
            list.append(mjs_asMesh(el));
            el = mjs_nextElement(self.ptr, el);
          }
          return list;
        },
        py::return_value_policy::reference_internal);
    

          mjSpec.def("add_skin", [](MjSpec& self, py::kwargs kwargs) -> raw::MjsSkin* {
            auto out = mjs_addSkin(self.ptr);
        
        std::set<std::string> valid_kwargs = {"name", "file", "material", "rgba", "inflate", "group", "vert", "texcoord", "face", "bodyname", "bindpos", "bindquat", "vertid", "vertweight", "info"};

        py::dict kwarg_dict = kwargs;
        for (auto item: kwarg_dict) {
          std::string key = py::str(item.first);
          if (valid_kwargs.count(key) == 0) {
            throw pybind11::type_error("Invalid "
              + key
              + " keyword argument. Valid options are: name, file, material, rgba, inflate, group, vert, texcoord, face, bodyname, bindpos, bindquat, vertid, vertweight, info.");
          }
        }
    

          auto set_vec = [&kwargs](const char* str, auto&& des) {
            if (kwargs.contains(str)) {
              try {
                using T = typename std::decay_t<decltype(*des)>::value_type;
                std::vector<T> vec = kwargs[str].cast<std::vector<T>>();
                des->clear();
                des->reserve(vec.size());
                for (auto val : vec) {
                  des->push_back(val);
                }
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " has the wrong type.");
              }
            }
          };
          

          auto set_array = [&kwargs](const char* str, auto&& des, int size) {
            if (kwargs.contains(str)) {
              try {
                using T = std::remove_pointer_t<std::decay_t<decltype(des)>>;
                std::vector<T> array = kwargs[str].cast<std::vector<T>>();
                if (array.size() != size) {
                  throw pybind11::value_error(std::string(str)
                    + " should be a list/array of size "
                    + std::to_string(size)
                    + ".");
                }
                int idx = 0;
                for (auto val : array) {
                  des[idx++] = val;
                }
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " should be a list/array.");
              }
            }
          };
          

          auto set_string = [&kwargs](const char* str, std::basic_string<char>* des) {
            if (kwargs.contains(str)) {
              try {
                *des = kwargs[str].cast<std::string>();
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " should be a string.");
              }
            }
          };
          

          auto set_value = [&kwargs](const char* str, auto&& des) {
            if (kwargs.contains(str)) {
              try {
                using T = std::decay_t<decltype(des)>;
                des = kwargs[str].cast<T>();
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " is the wrong type.");
              }
            }
          };
          
        set_string("name", out->name);
        set_string("file", out->file);
        set_string("material", out->material);
        set_array("rgba", out->rgba, 4);
        set_value("inflate", out->inflate);
        set_value("group", out->group);
        set_vec("vert", out->vert);
        set_vec("texcoord", out->texcoord);
        set_vec("face", out->face);
        set_vec("bodyname", out->bodyname);
        set_vec("bindpos", out->bindpos);
        set_vec("bindquat", out->bindquat);
        set_vec("vertid", out->vertid);
        set_vec("vertweight", out->vertweight);
        set_string("info", out->info);

        return out;
      },
      
      py::return_value_policy::reference_internal);
    

      mjSpec.def_property_readonly(
        "skins",
        [](MjSpec& self) -> py::list {
          py::list list;
          raw::MjsElement* el = mjs_firstElement(self.ptr, mjOBJ_SKIN);
          while (el) {
            list.append(mjs_asSkin(el));
            el = mjs_nextElement(self.ptr, el);
          }
          return list;
        },
        py::return_value_policy::reference_internal);
    

          mjSpec.def("add_hfield", [](MjSpec& self, py::kwargs kwargs) -> raw::MjsHField* {
            auto out = mjs_addHField(self.ptr);
        
        std::set<std::string> valid_kwargs = {"name", "content_type", "file", "size", "nrow", "ncol", "userdata", "info"};

        py::dict kwarg_dict = kwargs;
        for (auto item: kwarg_dict) {
          std::string key = py::str(item.first);
          if (valid_kwargs.count(key) == 0) {
            throw pybind11::type_error("Invalid "
              + key
              + " keyword argument. Valid options are: name, content_type, file, size, nrow, ncol, userdata, info.");
          }
        }
    

          auto set_vec = [&kwargs](const char* str, auto&& des) {
            if (kwargs.contains(str)) {
              try {
                using T = typename std::decay_t<decltype(*des)>::value_type;
                std::vector<T> vec = kwargs[str].cast<std::vector<T>>();
                des->clear();
                des->reserve(vec.size());
                for (auto val : vec) {
                  des->push_back(val);
                }
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " has the wrong type.");
              }
            }
          };
          

          auto set_array = [&kwargs](const char* str, auto&& des, int size) {
            if (kwargs.contains(str)) {
              try {
                using T = std::remove_pointer_t<std::decay_t<decltype(des)>>;
                std::vector<T> array = kwargs[str].cast<std::vector<T>>();
                if (array.size() != size) {
                  throw pybind11::value_error(std::string(str)
                    + " should be a list/array of size "
                    + std::to_string(size)
                    + ".");
                }
                int idx = 0;
                for (auto val : array) {
                  des[idx++] = val;
                }
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " should be a list/array.");
              }
            }
          };
          

          auto set_string = [&kwargs](const char* str, std::basic_string<char>* des) {
            if (kwargs.contains(str)) {
              try {
                *des = kwargs[str].cast<std::string>();
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " should be a string.");
              }
            }
          };
          

          auto set_value = [&kwargs](const char* str, auto&& des) {
            if (kwargs.contains(str)) {
              try {
                using T = std::decay_t<decltype(des)>;
                des = kwargs[str].cast<T>();
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " is the wrong type.");
              }
            }
          };
          
        set_string("name", out->name);
        set_string("content_type", out->content_type);
        set_string("file", out->file);
        set_array("size", out->size, 4);
        set_value("nrow", out->nrow);
        set_value("ncol", out->ncol);
        set_vec("userdata", out->userdata);
        set_string("info", out->info);

        return out;
      },
      
      py::return_value_policy::reference_internal);
    

      mjSpec.def_property_readonly(
        "hfields",
        [](MjSpec& self) -> py::list {
          py::list list;
          raw::MjsElement* el = mjs_firstElement(self.ptr, mjOBJ_HFIELD);
          while (el) {
            list.append(mjs_asHField(el));
            el = mjs_nextElement(self.ptr, el);
          }
          return list;
        },
        py::return_value_policy::reference_internal);
    

          mjSpec.def("add_texture", [](MjSpec& self, py::kwargs kwargs) -> raw::MjsTexture* {
            auto out = mjs_addTexture(self.ptr);
        
        std::set<std::string> valid_kwargs = {"name", "type", "builtin", "mark", "rgb1", "rgb2", "markrgb", "random", "height", "width", "nchannel", "content_type", "file", "gridsize", "gridlayout", "cubefiles", "data", "hflip", "vflip", "info"};

        py::dict kwarg_dict = kwargs;
        for (auto item: kwarg_dict) {
          std::string key = py::str(item.first);
          if (valid_kwargs.count(key) == 0) {
            throw pybind11::type_error("Invalid "
              + key
              + " keyword argument. Valid options are: name, type, builtin, mark, rgb1, rgb2, markrgb, random, height, width, nchannel, content_type, file, gridsize, gridlayout, cubefiles, data, hflip, vflip, info.");
          }
        }
    

          auto set_vec = [&kwargs](const char* str, auto&& des) {
            if (kwargs.contains(str)) {
              try {
                using T = typename std::decay_t<decltype(*des)>::value_type;
                std::vector<T> vec = kwargs[str].cast<std::vector<T>>();
                des->clear();
                des->reserve(vec.size());
                for (auto val : vec) {
                  des->push_back(val);
                }
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " has the wrong type.");
              }
            }
          };
          

          auto set_array = [&kwargs](const char* str, auto&& des, int size) {
            if (kwargs.contains(str)) {
              try {
                using T = std::remove_pointer_t<std::decay_t<decltype(des)>>;
                std::vector<T> array = kwargs[str].cast<std::vector<T>>();
                if (array.size() != size) {
                  throw pybind11::value_error(std::string(str)
                    + " should be a list/array of size "
                    + std::to_string(size)
                    + ".");
                }
                int idx = 0;
                for (auto val : array) {
                  des[idx++] = val;
                }
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " should be a list/array.");
              }
            }
          };
          

          auto set_string = [&kwargs](const char* str, std::basic_string<char>* des) {
            if (kwargs.contains(str)) {
              try {
                *des = kwargs[str].cast<std::string>();
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " should be a string.");
              }
            }
          };
          

          auto set_value = [&kwargs](const char* str, auto&& des) {
            if (kwargs.contains(str)) {
              try {
                using T = std::decay_t<decltype(des)>;
                des = kwargs[str].cast<T>();
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " is the wrong type.");
              }
            }
          };
          
        set_string("name", out->name);
        set_value("type", out->type);
        set_value("builtin", out->builtin);
        set_value("mark", out->mark);
        set_array("rgb1", out->rgb1, 3);
        set_array("rgb2", out->rgb2, 3);
        set_array("markrgb", out->markrgb, 3);
        set_value("random", out->random);
        set_value("height", out->height);
        set_value("width", out->width);
        set_value("nchannel", out->nchannel);
        set_string("content_type", out->content_type);
        set_string("file", out->file);
        set_array("gridsize", out->gridsize, 2);
        set_array("gridlayout", out->gridlayout, 13);
        set_vec("cubefiles", out->cubefiles);
        set_vec("data", out->data);
        set_value("hflip", out->hflip);
        set_value("vflip", out->vflip);
        set_string("info", out->info);

        return out;
      },
      
      py::return_value_policy::reference_internal);
    

      mjSpec.def_property_readonly(
        "textures",
        [](MjSpec& self) -> py::list {
          py::list list;
          raw::MjsElement* el = mjs_firstElement(self.ptr, mjOBJ_TEXTURE);
          while (el) {
            list.append(mjs_asTexture(el));
            el = mjs_nextElement(self.ptr, el);
          }
          return list;
        },
        py::return_value_policy::reference_internal);
    

          mjSpec.def("add_material", [](MjSpec& self,
            raw::MjsDefault* default_, py::kwargs kwargs) -> raw::MjsMaterial* {
            auto out = mjs_addMaterial(self.ptr, default_);
        
        std::set<std::string> valid_kwargs = {"name", "textures", "texuniform", "texrepeat", "emission", "specular", "shininess", "reflectance", "metallic", "roughness", "rgba", "info"};

        py::dict kwarg_dict = kwargs;
        for (auto item: kwarg_dict) {
          std::string key = py::str(item.first);
          if (valid_kwargs.count(key) == 0) {
            throw pybind11::type_error("Invalid "
              + key
              + " keyword argument. Valid options are: name, textures, texuniform, texrepeat, emission, specular, shininess, reflectance, metallic, roughness, rgba, info.");
          }
        }
    

          auto set_array = [&kwargs](const char* str, auto&& des, int size) {
            if (kwargs.contains(str)) {
              try {
                using T = std::remove_pointer_t<std::decay_t<decltype(des)>>;
                std::vector<T> array = kwargs[str].cast<std::vector<T>>();
                if (array.size() != size) {
                  throw pybind11::value_error(std::string(str)
                    + " should be a list/array of size "
                    + std::to_string(size)
                    + ".");
                }
                int idx = 0;
                for (auto val : array) {
                  des[idx++] = val;
                }
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " should be a list/array.");
              }
            }
          };
          

          auto set_value = [&kwargs](const char* str, auto&& des) {
            if (kwargs.contains(str)) {
              try {
                using T = std::decay_t<decltype(des)>;
                des = kwargs[str].cast<T>();
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " is the wrong type.");
              }
            }
          };
          

          auto set_string = [&kwargs](const char* str, std::basic_string<char>* des) {
            if (kwargs.contains(str)) {
              try {
                *des = kwargs[str].cast<std::string>();
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " should be a string.");
              }
            }
          };
          

          auto set_vec = [&kwargs](const char* str, auto&& des) {
            if (kwargs.contains(str)) {
              try {
                using T = typename std::decay_t<decltype(*des)>::value_type;
                std::vector<T> vec = kwargs[str].cast<std::vector<T>>();
                des->clear();
                des->reserve(vec.size());
                for (auto val : vec) {
                  des->push_back(val);
                }
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " has the wrong type.");
              }
            }
          };
          
        set_string("name", out->name);
        set_vec("textures", out->textures);
        set_value("texuniform", out->texuniform);
        set_array("texrepeat", out->texrepeat, 2);
        set_value("emission", out->emission);
        set_value("specular", out->specular);
        set_value("shininess", out->shininess);
        set_value("reflectance", out->reflectance);
        set_value("metallic", out->metallic);
        set_value("roughness", out->roughness);
        set_array("rgba", out->rgba, 4);
        set_string("info", out->info);

        return out;
      },
      py::arg_v("default", nullptr),
      py::return_value_policy::reference_internal);
    

      mjSpec.def_property_readonly(
        "materials",
        [](MjSpec& self) -> py::list {
          py::list list;
          raw::MjsElement* el = mjs_firstElement(self.ptr, mjOBJ_MATERIAL);
          while (el) {
            list.append(mjs_asMaterial(el));
            el = mjs_nextElement(self.ptr, el);
          }
          return list;
        },
        py::return_value_policy::reference_internal);
    

          mjSpec.def("add_pair", [](MjSpec& self,
            raw::MjsDefault* default_, py::kwargs kwargs) -> raw::MjsPair* {
            auto out = mjs_addPair(self.ptr, default_);
        
        std::set<std::string> valid_kwargs = {"name", "geomname1", "geomname2", "condim", "solref", "solreffriction", "solimp", "margin", "gap", "friction", "info"};

        py::dict kwarg_dict = kwargs;
        for (auto item: kwarg_dict) {
          std::string key = py::str(item.first);
          if (valid_kwargs.count(key) == 0) {
            throw pybind11::type_error("Invalid "
              + key
              + " keyword argument. Valid options are: name, geomname1, geomname2, condim, solref, solreffriction, solimp, margin, gap, friction, info.");
          }
        }
    

          auto set_array = [&kwargs](const char* str, auto&& des, int size) {
            if (kwargs.contains(str)) {
              try {
                using T = std::remove_pointer_t<std::decay_t<decltype(des)>>;
                std::vector<T> array = kwargs[str].cast<std::vector<T>>();
                if (array.size() != size) {
                  throw pybind11::value_error(std::string(str)
                    + " should be a list/array of size "
                    + std::to_string(size)
                    + ".");
                }
                int idx = 0;
                for (auto val : array) {
                  des[idx++] = val;
                }
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " should be a list/array.");
              }
            }
          };
          

          auto set_string = [&kwargs](const char* str, std::basic_string<char>* des) {
            if (kwargs.contains(str)) {
              try {
                *des = kwargs[str].cast<std::string>();
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " should be a string.");
              }
            }
          };
          

          auto set_value = [&kwargs](const char* str, auto&& des) {
            if (kwargs.contains(str)) {
              try {
                using T = std::decay_t<decltype(des)>;
                des = kwargs[str].cast<T>();
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " is the wrong type.");
              }
            }
          };
          
        set_string("name", out->name);
        set_string("geomname1", out->geomname1);
        set_string("geomname2", out->geomname2);
        set_value("condim", out->condim);
        set_array("solref", out->solref, 2);
        set_array("solreffriction", out->solreffriction, 2);
        set_array("solimp", out->solimp, 5);
        set_value("margin", out->margin);
        set_value("gap", out->gap);
        set_array("friction", out->friction, 5);
        set_string("info", out->info);

        return out;
      },
      py::arg_v("default", nullptr),
      py::return_value_policy::reference_internal);
    

      mjSpec.def_property_readonly(
        "pairs",
        [](MjSpec& self) -> py::list {
          py::list list;
          raw::MjsElement* el = mjs_firstElement(self.ptr, mjOBJ_PAIR);
          while (el) {
            list.append(mjs_asPair(el));
            el = mjs_nextElement(self.ptr, el);
          }
          return list;
        },
        py::return_value_policy::reference_internal);
    

          mjSpec.def("add_equality", [](MjSpec& self,
            raw::MjsDefault* default_, py::kwargs kwargs) -> raw::MjsEquality* {
            auto out = mjs_addEquality(self.ptr, default_);
        
        std::set<std::string> valid_kwargs = {"name", "type", "data", "active", "name1", "name2", "objtype", "solref", "solimp", "info"};

        py::dict kwarg_dict = kwargs;
        for (auto item: kwarg_dict) {
          std::string key = py::str(item.first);
          if (valid_kwargs.count(key) == 0) {
            throw pybind11::type_error("Invalid "
              + key
              + " keyword argument. Valid options are: name, type, data, active, name1, name2, objtype, solref, solimp, info.");
          }
        }
    

          auto set_array = [&kwargs](const char* str, auto&& des, int size) {
            if (kwargs.contains(str)) {
              try {
                using T = std::remove_pointer_t<std::decay_t<decltype(des)>>;
                std::vector<T> array = kwargs[str].cast<std::vector<T>>();
                if (array.size() != size) {
                  throw pybind11::value_error(std::string(str)
                    + " should be a list/array of size "
                    + std::to_string(size)
                    + ".");
                }
                int idx = 0;
                for (auto val : array) {
                  des[idx++] = val;
                }
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " should be a list/array.");
              }
            }
          };
          

          auto set_string = [&kwargs](const char* str, std::basic_string<char>* des) {
            if (kwargs.contains(str)) {
              try {
                *des = kwargs[str].cast<std::string>();
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " should be a string.");
              }
            }
          };
          

          auto set_value = [&kwargs](const char* str, auto&& des) {
            if (kwargs.contains(str)) {
              try {
                using T = std::decay_t<decltype(des)>;
                des = kwargs[str].cast<T>();
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " is the wrong type.");
              }
            }
          };
          
        set_string("name", out->name);
        set_value("type", out->type);
        set_array("data", out->data, 11);
        set_value("active", out->active);
        set_string("name1", out->name1);
        set_string("name2", out->name2);
        set_value("objtype", out->objtype);
        set_array("solref", out->solref, 2);
        set_array("solimp", out->solimp, 5);
        set_string("info", out->info);

        return out;
      },
      py::arg_v("default", nullptr),
      py::return_value_policy::reference_internal);
    

      mjSpec.def_property_readonly(
        "equalities",
        [](MjSpec& self) -> py::list {
          py::list list;
          raw::MjsElement* el = mjs_firstElement(self.ptr, mjOBJ_EQUALITY);
          while (el) {
            list.append(mjs_asEquality(el));
            el = mjs_nextElement(self.ptr, el);
          }
          return list;
        },
        py::return_value_policy::reference_internal);
    

          mjSpec.def("add_tendon", [](MjSpec& self,
            raw::MjsDefault* default_, py::kwargs kwargs) -> raw::MjsTendon* {
            auto out = mjs_addTendon(self.ptr, default_);
        
        std::set<std::string> valid_kwargs = {"name", "stiffness", "springlength", "damping", "frictionloss", "solref_friction", "solimp_friction", "limited", "range", "margin", "solref_limit", "solimp_limit", "material", "width", "rgba", "group", "userdata", "info"};

        py::dict kwarg_dict = kwargs;
        for (auto item: kwarg_dict) {
          std::string key = py::str(item.first);
          if (valid_kwargs.count(key) == 0) {
            throw pybind11::type_error("Invalid "
              + key
              + " keyword argument. Valid options are: name, stiffness, springlength, damping, frictionloss, solref_friction, solimp_friction, limited, range, margin, solref_limit, solimp_limit, material, width, rgba, group, userdata, info.");
          }
        }
    

          auto set_vec = [&kwargs](const char* str, auto&& des) {
            if (kwargs.contains(str)) {
              try {
                using T = typename std::decay_t<decltype(*des)>::value_type;
                std::vector<T> vec = kwargs[str].cast<std::vector<T>>();
                des->clear();
                des->reserve(vec.size());
                for (auto val : vec) {
                  des->push_back(val);
                }
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " has the wrong type.");
              }
            }
          };
          

          auto set_array = [&kwargs](const char* str, auto&& des, int size) {
            if (kwargs.contains(str)) {
              try {
                using T = std::remove_pointer_t<std::decay_t<decltype(des)>>;
                std::vector<T> array = kwargs[str].cast<std::vector<T>>();
                if (array.size() != size) {
                  throw pybind11::value_error(std::string(str)
                    + " should be a list/array of size "
                    + std::to_string(size)
                    + ".");
                }
                int idx = 0;
                for (auto val : array) {
                  des[idx++] = val;
                }
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " should be a list/array.");
              }
            }
          };
          

          auto set_string = [&kwargs](const char* str, std::basic_string<char>* des) {
            if (kwargs.contains(str)) {
              try {
                *des = kwargs[str].cast<std::string>();
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " should be a string.");
              }
            }
          };
          

          auto set_value = [&kwargs](const char* str, auto&& des) {
            if (kwargs.contains(str)) {
              try {
                using T = std::decay_t<decltype(des)>;
                des = kwargs[str].cast<T>();
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " is the wrong type.");
              }
            }
          };
          
        set_string("name", out->name);
        set_value("stiffness", out->stiffness);
        set_array("springlength", out->springlength, 2);
        set_value("damping", out->damping);
        set_value("frictionloss", out->frictionloss);
        set_array("solref_friction", out->solref_friction, 2);
        set_array("solimp_friction", out->solimp_friction, 5);
        set_value("limited", out->limited);
        set_array("range", out->range, 2);
        set_value("margin", out->margin);
        set_array("solref_limit", out->solref_limit, 2);
        set_array("solimp_limit", out->solimp_limit, 5);
        set_string("material", out->material);
        set_value("width", out->width);
        set_array("rgba", out->rgba, 4);
        set_value("group", out->group);
        set_vec("userdata", out->userdata);
        set_string("info", out->info);

        return out;
      },
      py::arg_v("default", nullptr),
      py::return_value_policy::reference_internal);
    

      mjSpec.def_property_readonly(
        "tendons",
        [](MjSpec& self) -> py::list {
          py::list list;
          raw::MjsElement* el = mjs_firstElement(self.ptr, mjOBJ_TENDON);
          while (el) {
            list.append(mjs_asTendon(el));
            el = mjs_nextElement(self.ptr, el);
          }
          return list;
        },
        py::return_value_policy::reference_internal);
    

          mjSpec.def("add_actuator", [](MjSpec& self,
            raw::MjsDefault* default_, py::kwargs kwargs) -> raw::MjsActuator* {
            auto out = mjs_addActuator(self.ptr, default_);
        
        std::set<std::string> valid_kwargs = {"name", "gaintype", "gainprm", "biastype", "biasprm", "dyntype", "dynprm", "actdim", "actearly", "trntype", "gear", "target", "refsite", "slidersite", "cranklength", "lengthrange", "inheritrange", "ctrllimited", "ctrlrange", "forcelimited", "forcerange", "actlimited", "actrange", "group", "userdata", "plugin", "info"};

        py::dict kwarg_dict = kwargs;
        for (auto item: kwarg_dict) {
          std::string key = py::str(item.first);
          if (valid_kwargs.count(key) == 0) {
            throw pybind11::type_error("Invalid "
              + key
              + " keyword argument. Valid options are: name, gaintype, gainprm, biastype, biasprm, dyntype, dynprm, actdim, actearly, trntype, gear, target, refsite, slidersite, cranklength, lengthrange, inheritrange, ctrllimited, ctrlrange, forcelimited, forcerange, actlimited, actrange, group, userdata, plugin, info.");
          }
        }
    

          auto set_string = [&kwargs](const char* str, std::basic_string<char>* des) {
            if (kwargs.contains(str)) {
              try {
                *des = kwargs[str].cast<std::string>();
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " should be a string.");
              }
            }
          };
          

          auto set_vec = [&kwargs](const char* str, auto&& des) {
            if (kwargs.contains(str)) {
              try {
                using T = typename std::decay_t<decltype(*des)>::value_type;
                std::vector<T> vec = kwargs[str].cast<std::vector<T>>();
                des->clear();
                des->reserve(vec.size());
                for (auto val : vec) {
                  des->push_back(val);
                }
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " has the wrong type.");
              }
            }
          };
          

          auto set_value = [&kwargs](const char* str, auto&& des) {
            if (kwargs.contains(str)) {
              try {
                using T = std::decay_t<decltype(des)>;
                des = kwargs[str].cast<T>();
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " is the wrong type.");
              }
            }
          };
          

          auto set_array = [&kwargs](const char* str, auto&& des, int size) {
            if (kwargs.contains(str)) {
              try {
                using T = std::remove_pointer_t<std::decay_t<decltype(des)>>;
                std::vector<T> array = kwargs[str].cast<std::vector<T>>();
                if (array.size() != size) {
                  throw pybind11::value_error(std::string(str)
                    + " should be a list/array of size "
                    + std::to_string(size)
                    + ".");
                }
                int idx = 0;
                for (auto val : array) {
                  des[idx++] = val;
                }
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " should be a list/array.");
              }
            }
          };
          

          auto set_plugin = [&kwargs](raw::MjsPlugin& plugin) {
            if (kwargs.contains("plugin")) {
              std::optional<raw::MjsPlugin> input = kwargs["plugin"].cast<raw::MjsPlugin>();
              if (input.has_value()) {
                try {
                  plugin.name = input->name;
                } catch (const py::cast_error &e) {
                  throw pybind11::value_error("plugin.name should be a string.");
                }
                try {
                  plugin.plugin_name = input->plugin_name;
                } catch (const py::cast_error &e) {
                  throw pybind11::value_error("plugin.instance_name should be a string.");
                }
                try {
                  plugin.active = input->active;
                } catch (const py::cast_error &e) {
                  throw pybind11::value_error("plugin.active should be an mjtByte.");
                }
                try {
                  plugin.info = input->info;
                } catch (const py::cast_error &e) {
                  throw pybind11::value_error("plugin.info should be a string.");
                }
              }
            }
          };
          
        set_string("name", out->name);
        set_value("gaintype", out->gaintype);
        set_array("gainprm", out->gainprm, 10);
        set_value("biastype", out->biastype);
        set_array("biasprm", out->biasprm, 10);
        set_value("dyntype", out->dyntype);
        set_array("dynprm", out->dynprm, 10);
        set_value("actdim", out->actdim);
        set_value("actearly", out->actearly);
        set_value("trntype", out->trntype);
        set_array("gear", out->gear, 6);
        set_string("target", out->target);
        set_string("refsite", out->refsite);
        set_string("slidersite", out->slidersite);
        set_value("cranklength", out->cranklength);
        set_array("lengthrange", out->lengthrange, 2);
        set_value("inheritrange", out->inheritrange);
        set_value("ctrllimited", out->ctrllimited);
        set_array("ctrlrange", out->ctrlrange, 2);
        set_value("forcelimited", out->forcelimited);
        set_array("forcerange", out->forcerange, 2);
        set_value("actlimited", out->actlimited);
        set_array("actrange", out->actrange, 2);
        set_value("group", out->group);
        set_vec("userdata", out->userdata);
        set_plugin(out->plugin);
        set_string("info", out->info);

        return out;
      },
      py::arg_v("default", nullptr),
      py::return_value_policy::reference_internal);
    

      mjSpec.def_property_readonly(
        "actuators",
        [](MjSpec& self) -> py::list {
          py::list list;
          raw::MjsElement* el = mjs_firstElement(self.ptr, mjOBJ_ACTUATOR);
          while (el) {
            list.append(mjs_asActuator(el));
            el = mjs_nextElement(self.ptr, el);
          }
          return list;
        },
        py::return_value_policy::reference_internal);
    

          mjSpec.def("add_sensor", [](MjSpec& self, py::kwargs kwargs) -> raw::MjsSensor* {
            auto out = mjs_addSensor(self.ptr);
        
        std::set<std::string> valid_kwargs = {"name", "type", "objtype", "objname", "reftype", "refname", "datatype", "needstage", "dim", "cutoff", "noise", "userdata", "plugin", "info"};

        py::dict kwarg_dict = kwargs;
        for (auto item: kwarg_dict) {
          std::string key = py::str(item.first);
          if (valid_kwargs.count(key) == 0) {
            throw pybind11::type_error("Invalid "
              + key
              + " keyword argument. Valid options are: name, type, objtype, objname, reftype, refname, datatype, needstage, dim, cutoff, noise, userdata, plugin, info.");
          }
        }
    

          auto set_vec = [&kwargs](const char* str, auto&& des) {
            if (kwargs.contains(str)) {
              try {
                using T = typename std::decay_t<decltype(*des)>::value_type;
                std::vector<T> vec = kwargs[str].cast<std::vector<T>>();
                des->clear();
                des->reserve(vec.size());
                for (auto val : vec) {
                  des->push_back(val);
                }
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " has the wrong type.");
              }
            }
          };
          

          auto set_string = [&kwargs](const char* str, std::basic_string<char>* des) {
            if (kwargs.contains(str)) {
              try {
                *des = kwargs[str].cast<std::string>();
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " should be a string.");
              }
            }
          };
          

          auto set_value = [&kwargs](const char* str, auto&& des) {
            if (kwargs.contains(str)) {
              try {
                using T = std::decay_t<decltype(des)>;
                des = kwargs[str].cast<T>();
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " is the wrong type.");
              }
            }
          };
          

          auto set_plugin = [&kwargs](raw::MjsPlugin& plugin) {
            if (kwargs.contains("plugin")) {
              std::optional<raw::MjsPlugin> input = kwargs["plugin"].cast<raw::MjsPlugin>();
              if (input.has_value()) {
                try {
                  plugin.name = input->name;
                } catch (const py::cast_error &e) {
                  throw pybind11::value_error("plugin.name should be a string.");
                }
                try {
                  plugin.plugin_name = input->plugin_name;
                } catch (const py::cast_error &e) {
                  throw pybind11::value_error("plugin.instance_name should be a string.");
                }
                try {
                  plugin.active = input->active;
                } catch (const py::cast_error &e) {
                  throw pybind11::value_error("plugin.active should be an mjtByte.");
                }
                try {
                  plugin.info = input->info;
                } catch (const py::cast_error &e) {
                  throw pybind11::value_error("plugin.info should be a string.");
                }
              }
            }
          };
          
        set_string("name", out->name);
        set_value("type", out->type);
        set_value("objtype", out->objtype);
        set_string("objname", out->objname);
        set_value("reftype", out->reftype);
        set_string("refname", out->refname);
        set_value("datatype", out->datatype);
        set_value("needstage", out->needstage);
        set_value("dim", out->dim);
        set_value("cutoff", out->cutoff);
        set_value("noise", out->noise);
        set_vec("userdata", out->userdata);
        set_plugin(out->plugin);
        set_string("info", out->info);

        return out;
      },
      
      py::return_value_policy::reference_internal);
    

      mjSpec.def_property_readonly(
        "sensors",
        [](MjSpec& self) -> py::list {
          py::list list;
          raw::MjsElement* el = mjs_firstElement(self.ptr, mjOBJ_SENSOR);
          while (el) {
            list.append(mjs_asSensor(el));
            el = mjs_nextElement(self.ptr, el);
          }
          return list;
        },
        py::return_value_policy::reference_internal);
    

          mjSpec.def("add_numeric", [](MjSpec& self, py::kwargs kwargs) -> raw::MjsNumeric* {
            auto out = mjs_addNumeric(self.ptr);
        
        std::set<std::string> valid_kwargs = {"name", "data", "size", "info"};

        py::dict kwarg_dict = kwargs;
        for (auto item: kwarg_dict) {
          std::string key = py::str(item.first);
          if (valid_kwargs.count(key) == 0) {
            throw pybind11::type_error("Invalid "
              + key
              + " keyword argument. Valid options are: name, data, size, info.");
          }
        }
    

          auto set_value = [&kwargs](const char* str, auto&& des) {
            if (kwargs.contains(str)) {
              try {
                using T = std::decay_t<decltype(des)>;
                des = kwargs[str].cast<T>();
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " is the wrong type.");
              }
            }
          };
          

          auto set_string = [&kwargs](const char* str, std::basic_string<char>* des) {
            if (kwargs.contains(str)) {
              try {
                *des = kwargs[str].cast<std::string>();
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " should be a string.");
              }
            }
          };
          

          auto set_vec = [&kwargs](const char* str, auto&& des) {
            if (kwargs.contains(str)) {
              try {
                using T = typename std::decay_t<decltype(*des)>::value_type;
                std::vector<T> vec = kwargs[str].cast<std::vector<T>>();
                des->clear();
                des->reserve(vec.size());
                for (auto val : vec) {
                  des->push_back(val);
                }
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " has the wrong type.");
              }
            }
          };
          
        set_string("name", out->name);
        set_vec("data", out->data);
        set_value("size", out->size);
        set_string("info", out->info);

        return out;
      },
      
      py::return_value_policy::reference_internal);
    

      mjSpec.def_property_readonly(
        "numerics",
        [](MjSpec& self) -> py::list {
          py::list list;
          raw::MjsElement* el = mjs_firstElement(self.ptr, mjOBJ_NUMERIC);
          while (el) {
            list.append(mjs_asNumeric(el));
            el = mjs_nextElement(self.ptr, el);
          }
          return list;
        },
        py::return_value_policy::reference_internal);
    

          mjSpec.def("add_text", [](MjSpec& self, py::kwargs kwargs) -> raw::MjsText* {
            auto out = mjs_addText(self.ptr);
        
        std::set<std::string> valid_kwargs = {"name", "data", "info"};

        py::dict kwarg_dict = kwargs;
        for (auto item: kwarg_dict) {
          std::string key = py::str(item.first);
          if (valid_kwargs.count(key) == 0) {
            throw pybind11::type_error("Invalid "
              + key
              + " keyword argument. Valid options are: name, data, info.");
          }
        }
    

          auto set_string = [&kwargs](const char* str, std::basic_string<char>* des) {
            if (kwargs.contains(str)) {
              try {
                *des = kwargs[str].cast<std::string>();
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " should be a string.");
              }
            }
          };
          
        set_string("name", out->name);
        set_string("data", out->data);
        set_string("info", out->info);

        return out;
      },
      
      py::return_value_policy::reference_internal);
    

      mjSpec.def_property_readonly(
        "texts",
        [](MjSpec& self) -> py::list {
          py::list list;
          raw::MjsElement* el = mjs_firstElement(self.ptr, mjOBJ_TEXT);
          while (el) {
            list.append(mjs_asText(el));
            el = mjs_nextElement(self.ptr, el);
          }
          return list;
        },
        py::return_value_policy::reference_internal);
    

          mjSpec.def("add_tuple", [](MjSpec& self, py::kwargs kwargs) -> raw::MjsTuple* {
            auto out = mjs_addTuple(self.ptr);
        
        std::set<std::string> valid_kwargs = {"name", "objtype", "objname", "objprm", "info"};

        py::dict kwarg_dict = kwargs;
        for (auto item: kwarg_dict) {
          std::string key = py::str(item.first);
          if (valid_kwargs.count(key) == 0) {
            throw pybind11::type_error("Invalid "
              + key
              + " keyword argument. Valid options are: name, objtype, objname, objprm, info.");
          }
        }
    

          auto set_string = [&kwargs](const char* str, std::basic_string<char>* des) {
            if (kwargs.contains(str)) {
              try {
                *des = kwargs[str].cast<std::string>();
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " should be a string.");
              }
            }
          };
          

          auto set_vec = [&kwargs](const char* str, auto&& des) {
            if (kwargs.contains(str)) {
              try {
                using T = typename std::decay_t<decltype(*des)>::value_type;
                std::vector<T> vec = kwargs[str].cast<std::vector<T>>();
                des->clear();
                des->reserve(vec.size());
                for (auto val : vec) {
                  des->push_back(val);
                }
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " has the wrong type.");
              }
            }
          };
          
        set_string("name", out->name);
        set_vec("objtype", out->objtype);
        set_vec("objname", out->objname);
        set_vec("objprm", out->objprm);
        set_string("info", out->info);

        return out;
      },
      
      py::return_value_policy::reference_internal);
    

      mjSpec.def_property_readonly(
        "tuples",
        [](MjSpec& self) -> py::list {
          py::list list;
          raw::MjsElement* el = mjs_firstElement(self.ptr, mjOBJ_TUPLE);
          while (el) {
            list.append(mjs_asTuple(el));
            el = mjs_nextElement(self.ptr, el);
          }
          return list;
        },
        py::return_value_policy::reference_internal);
    

          mjSpec.def("add_key", [](MjSpec& self, py::kwargs kwargs) -> raw::MjsKey* {
            auto out = mjs_addKey(self.ptr);
        
        std::set<std::string> valid_kwargs = {"name", "time", "qpos", "qvel", "act", "mpos", "mquat", "ctrl", "info"};

        py::dict kwarg_dict = kwargs;
        for (auto item: kwarg_dict) {
          std::string key = py::str(item.first);
          if (valid_kwargs.count(key) == 0) {
            throw pybind11::type_error("Invalid "
              + key
              + " keyword argument. Valid options are: name, time, qpos, qvel, act, mpos, mquat, ctrl, info.");
          }
        }
    

          auto set_vec = [&kwargs](const char* str, auto&& des) {
            if (kwargs.contains(str)) {
              try {
                using T = typename std::decay_t<decltype(*des)>::value_type;
                std::vector<T> vec = kwargs[str].cast<std::vector<T>>();
                des->clear();
                des->reserve(vec.size());
                for (auto val : vec) {
                  des->push_back(val);
                }
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " has the wrong type.");
              }
            }
          };
          

          auto set_string = [&kwargs](const char* str, std::basic_string<char>* des) {
            if (kwargs.contains(str)) {
              try {
                *des = kwargs[str].cast<std::string>();
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " should be a string.");
              }
            }
          };
          

          auto set_value = [&kwargs](const char* str, auto&& des) {
            if (kwargs.contains(str)) {
              try {
                using T = std::decay_t<decltype(des)>;
                des = kwargs[str].cast<T>();
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " is the wrong type.");
              }
            }
          };
          
        set_string("name", out->name);
        set_value("time", out->time);
        set_vec("qpos", out->qpos);
        set_vec("qvel", out->qvel);
        set_vec("act", out->act);
        set_vec("mpos", out->mpos);
        set_vec("mquat", out->mquat);
        set_vec("ctrl", out->ctrl);
        set_string("info", out->info);

        return out;
      },
      
      py::return_value_policy::reference_internal);
    

      mjSpec.def_property_readonly(
        "keys",
        [](MjSpec& self) -> py::list {
          py::list list;
          raw::MjsElement* el = mjs_firstElement(self.ptr, mjOBJ_KEY);
          while (el) {
            list.append(mjs_asKey(el));
            el = mjs_nextElement(self.ptr, el);
          }
          return list;
        },
        py::return_value_policy::reference_internal);
    

          mjSpec.def("add_exclude", [](MjSpec& self, py::kwargs kwargs) -> raw::MjsExclude* {
            auto out = mjs_addExclude(self.ptr);
        
        std::set<std::string> valid_kwargs = {"name", "bodyname1", "bodyname2", "info"};

        py::dict kwarg_dict = kwargs;
        for (auto item: kwarg_dict) {
          std::string key = py::str(item.first);
          if (valid_kwargs.count(key) == 0) {
            throw pybind11::type_error("Invalid "
              + key
              + " keyword argument. Valid options are: name, bodyname1, bodyname2, info.");
          }
        }
    

          auto set_string = [&kwargs](const char* str, std::basic_string<char>* des) {
            if (kwargs.contains(str)) {
              try {
                *des = kwargs[str].cast<std::string>();
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " should be a string.");
              }
            }
          };
          
        set_string("name", out->name);
        set_string("bodyname1", out->bodyname1);
        set_string("bodyname2", out->bodyname2);
        set_string("info", out->info);

        return out;
      },
      
      py::return_value_policy::reference_internal);
    

      mjSpec.def_property_readonly(
        "excludes",
        [](MjSpec& self) -> py::list {
          py::list list;
          raw::MjsElement* el = mjs_firstElement(self.ptr, mjOBJ_EXCLUDE);
          while (el) {
            list.append(mjs_asExclude(el));
            el = mjs_nextElement(self.ptr, el);
          }
          return list;
        },
        py::return_value_policy::reference_internal);
    

          mjSpec.def("add_plugin", [](MjSpec& self, py::kwargs kwargs) -> raw::MjsPlugin* {
            auto out = mjs_addPlugin(self.ptr);
        
        std::set<std::string> valid_kwargs = {"name", "plugin_name", "active", "info"};

        py::dict kwarg_dict = kwargs;
        for (auto item: kwarg_dict) {
          std::string key = py::str(item.first);
          if (valid_kwargs.count(key) == 0) {
            throw pybind11::type_error("Invalid "
              + key
              + " keyword argument. Valid options are: name, plugin_name, active, info.");
          }
        }
    

          auto set_string = [&kwargs](const char* str, std::basic_string<char>* des) {
            if (kwargs.contains(str)) {
              try {
                *des = kwargs[str].cast<std::string>();
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " should be a string.");
              }
            }
          };
          

          auto set_value = [&kwargs](const char* str, auto&& des) {
            if (kwargs.contains(str)) {
              try {
                using T = std::decay_t<decltype(des)>;
                des = kwargs[str].cast<T>();
              } catch (const py::cast_error &e) {
                throw pybind11::value_error(std::string(str) + " is the wrong type.");
              }
            }
          };
          
        set_string("name", out->name);
        set_string("plugin_name", out->plugin_name);
        set_value("active", out->active);
        set_string("info", out->info);

        return out;
      },
      
      py::return_value_policy::reference_internal);
    

      mjSpec.def_property_readonly(
        "plugins",
        [](MjSpec& self) -> py::list {
          py::list list;
          raw::MjsElement* el = mjs_firstElement(self.ptr, mjOBJ_PLUGIN);
          while (el) {
            list.append(mjs_asPlugin(el));
            el = mjs_nextElement(self.ptr, el);
          }
          return list;
        },
        py::return_value_policy::reference_internal);
    


      mjSpec.def("body",
      [](MjSpec& self, std::string& name) -> raw::MjsBody* {
        return mjs_asBody(
            mjs_findElement(self.ptr, mjOBJ_BODY, name.c_str()));
      }, py::return_value_policy::reference_internal);
    


      mjSpec.def("site",
      [](MjSpec& self, std::string& name) -> raw::MjsSite* {
        return mjs_asSite(
            mjs_findElement(self.ptr, mjOBJ_SITE, name.c_str()));
      }, py::return_value_policy::reference_internal);
    


      mjSpec.def("geom",
      [](MjSpec& self, std::string& name) -> raw::MjsGeom* {
        return mjs_asGeom(
            mjs_findElement(self.ptr, mjOBJ_GEOM, name.c_str()));
      }, py::return_value_policy::reference_internal);
    


      mjSpec.def("joint",
      [](MjSpec& self, std::string& name) -> raw::MjsJoint* {
        return mjs_asJoint(
            mjs_findElement(self.ptr, mjOBJ_JOINT, name.c_str()));
      }, py::return_value_policy::reference_internal);
    


      mjSpec.def("camera",
      [](MjSpec& self, std::string& name) -> raw::MjsCamera* {
        return mjs_asCamera(
            mjs_findElement(self.ptr, mjOBJ_CAMERA, name.c_str()));
      }, py::return_value_policy::reference_internal);
    


      mjSpec.def("frame",
      [](MjSpec& self, std::string& name) -> raw::MjsFrame* {
        return mjs_asFrame(
            mjs_findElement(self.ptr, mjOBJ_FRAME, name.c_str()));
      }, py::return_value_policy::reference_internal);
    


      mjSpec.def("light",
      [](MjSpec& self, std::string& name) -> raw::MjsLight* {
        return mjs_asLight(
            mjs_findElement(self.ptr, mjOBJ_LIGHT, name.c_str()));
      }, py::return_value_policy::reference_internal);
    


      mjSpec.def("flex",
      [](MjSpec& self, std::string& name) -> raw::MjsFlex* {
        return mjs_asFlex(
            mjs_findElement(self.ptr, mjOBJ_FLEX, name.c_str()));
      }, py::return_value_policy::reference_internal);
    


      mjSpec.def("mesh",
      [](MjSpec& self, std::string& name) -> raw::MjsMesh* {
        return mjs_asMesh(
            mjs_findElement(self.ptr, mjOBJ_MESH, name.c_str()));
      }, py::return_value_policy::reference_internal);
    


      mjSpec.def("skin",
      [](MjSpec& self, std::string& name) -> raw::MjsSkin* {
        return mjs_asSkin(
            mjs_findElement(self.ptr, mjOBJ_SKIN, name.c_str()));
      }, py::return_value_policy::reference_internal);
    


      mjSpec.def("hfield",
      [](MjSpec& self, std::string& name) -> raw::MjsHField* {
        return mjs_asHField(
            mjs_findElement(self.ptr, mjOBJ_HFIELD, name.c_str()));
      }, py::return_value_policy::reference_internal);
    


      mjSpec.def("texture",
      [](MjSpec& self, std::string& name) -> raw::MjsTexture* {
        return mjs_asTexture(
            mjs_findElement(self.ptr, mjOBJ_TEXTURE, name.c_str()));
      }, py::return_value_policy::reference_internal);
    


      mjSpec.def("material",
      [](MjSpec& self, std::string& name) -> raw::MjsMaterial* {
        return mjs_asMaterial(
            mjs_findElement(self.ptr, mjOBJ_MATERIAL, name.c_str()));
      }, py::return_value_policy::reference_internal);
    


      mjSpec.def("pair",
      [](MjSpec& self, std::string& name) -> raw::MjsPair* {
        return mjs_asPair(
            mjs_findElement(self.ptr, mjOBJ_PAIR, name.c_str()));
      }, py::return_value_policy::reference_internal);
    


      mjSpec.def("equality",
      [](MjSpec& self, std::string& name) -> raw::MjsEquality* {
        return mjs_asEquality(
            mjs_findElement(self.ptr, mjOBJ_EQUALITY, name.c_str()));
      }, py::return_value_policy::reference_internal);
    


      mjSpec.def("tendon",
      [](MjSpec& self, std::string& name) -> raw::MjsTendon* {
        return mjs_asTendon(
            mjs_findElement(self.ptr, mjOBJ_TENDON, name.c_str()));
      }, py::return_value_policy::reference_internal);
    


      mjSpec.def("actuator",
      [](MjSpec& self, std::string& name) -> raw::MjsActuator* {
        return mjs_asActuator(
            mjs_findElement(self.ptr, mjOBJ_ACTUATOR, name.c_str()));
      }, py::return_value_policy::reference_internal);
    


      mjSpec.def("sensor",
      [](MjSpec& self, std::string& name) -> raw::MjsSensor* {
        return mjs_asSensor(
            mjs_findElement(self.ptr, mjOBJ_SENSOR, name.c_str()));
      }, py::return_value_policy::reference_internal);
    


      mjSpec.def("numeric",
      [](MjSpec& self, std::string& name) -> raw::MjsNumeric* {
        return mjs_asNumeric(
            mjs_findElement(self.ptr, mjOBJ_NUMERIC, name.c_str()));
      }, py::return_value_policy::reference_internal);
    


      mjSpec.def("text",
      [](MjSpec& self, std::string& name) -> raw::MjsText* {
        return mjs_asText(
            mjs_findElement(self.ptr, mjOBJ_TEXT, name.c_str()));
      }, py::return_value_policy::reference_internal);
    


      mjSpec.def("tuple",
      [](MjSpec& self, std::string& name) -> raw::MjsTuple* {
        return mjs_asTuple(
            mjs_findElement(self.ptr, mjOBJ_TUPLE, name.c_str()));
      }, py::return_value_policy::reference_internal);
    


      mjSpec.def("key",
      [](MjSpec& self, std::string& name) -> raw::MjsKey* {
        return mjs_asKey(
            mjs_findElement(self.ptr, mjOBJ_KEY, name.c_str()));
      }, py::return_value_policy::reference_internal);
    


      mjSpec.def("exclude",
      [](MjSpec& self, std::string& name) -> raw::MjsExclude* {
        return mjs_asExclude(
            mjs_findElement(self.ptr, mjOBJ_EXCLUDE, name.c_str()));
      }, py::return_value_policy::reference_internal);
    


      mjSpec.def("plugin",
      [](MjSpec& self, std::string& name) -> raw::MjsPlugin* {
        return mjs_asPlugin(
            mjs_findElement(self.ptr, mjOBJ_PLUGIN, name.c_str()));
      }, py::return_value_policy::reference_internal);
    

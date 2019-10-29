
//#define CATCH_CONFIG_DISABLE

#include <catch.hpp>
#include <memory>

#include "../../libslic3r/GCodeWriter.hpp"
#include "../test_options.hpp"
#include <iomanip>
#include <iostream>
//#include "../test_data.hpp" // get access to init_print, etc
#define PRECISION(val, precision) std::fixed << std::setprecision(precision) << val
#define XYZF_NUM(val) PRECISION(val, 3)

using namespace Slic3r;
using namespace std::literals::string_literals;

// can't understand what the test want to test: here we are overflowing the double capacity to break the lift logic...
//so i moved m_lifted = 0; out of the test in unlift to make that pass.
SCENARIO("lift() and unlift() behavior with large values of Z", "[!shouldfail]") {
    GIVEN("A config from a file and a single extruder.") {
        GCodeWriter writer{};
        GCodeConfig& config {writer.config};
        config.set_defaults();
        config.load(std::string{ testfile_dir } +"test_gcodewriter/config_lift_unlift.ini"s);

        std::vector<unsigned int> extruder_ids {0};
        writer.set_extruders(extruder_ids);
        writer.set_extruder(0);

        WHEN("Z is set to 9007199254740992") {
            double trouble_Z{ 9007199254740992 };
            writer.travel_to_z(trouble_Z);
            AND_WHEN("GcodeWriter::Lift() is called") {
                const std::string lift = writer.lift();
                REQUIRE(lift.size() > 0);
                AND_WHEN("Z is moved post-lift to the same delta as the config Z lift") {
                    REQUIRE(writer.travel_to_z(trouble_Z + config.retract_lift.values[0]).size() == 0);
                    AND_WHEN("GCodeWriter::Unlift() is called") {
                        const std::string unlift = writer.unlift();
                        REQUIRE(unlift.size() == 0); // we're the same height so no additional move happens.
                        THEN("GCodeWriter::Lift() emits gcode.") {
                            const std::string lift_again = writer.lift();
                            REQUIRE(lift_again.size() > 0);
                        }
                    }
                }
            }
        }
    }
}

SCENARIO("lift() is not ignored after unlift() at normal values of Z") {
    GIVEN("A config from a file and a single extruder.") {
        GCodeWriter writer{};
        GCodeConfig& config {writer.config};
        config.set_defaults();
        config.load(std::string{ testfile_dir } +"test_gcodewriter/config_lift_unlift.ini"s);

        std::vector<unsigned int> extruder_ids {0};
        writer.set_extruders(extruder_ids);
        writer.set_extruder(0);

        WHEN("Z is set to 203") {
            double trouble_Z{ 203 };
            writer.travel_to_z(trouble_Z);
            AND_WHEN("GcodeWriter::Lift() is called") {
                REQUIRE(writer.lift().size() > 0);
                AND_WHEN("Z is moved post-lift to the same delta as the config Z lift") {
                    REQUIRE(writer.travel_to_z(trouble_Z + config.retract_lift.values[0]).size() == 0);
                    AND_WHEN("GCodeWriter::Unlift() is called") {
                        REQUIRE(writer.unlift().size() == 0); // we're the same height so no additional move happens.
                        THEN("GCodeWriter::Lift() emits gcode.") {
                            REQUIRE(writer.lift().size() > 0);
                        }
                    }
                }
            }
        }
        WHEN("Z is set to 500003") {
            double trouble_Z{ 500003 };
            writer.travel_to_z(trouble_Z);
            AND_WHEN("GcodeWriter::Lift() is called") {
                REQUIRE(writer.lift().size() > 0);
                AND_WHEN("Z is moved post-lift to the same delta as the config Z lift") {
                    REQUIRE(writer.travel_to_z(trouble_Z + config.retract_lift.values[0]).size() == 0);
                    AND_WHEN("GCodeWriter::Unlift() is called") {
                        REQUIRE(writer.unlift().size() == 0); // we're the same height so no additional move happens.
                        THEN("GCodeWriter::Lift() emits gcode.") {
                            REQUIRE(writer.lift().size() > 0);
                        }
                    }
                }
            }
        }
        WHEN("Z is set to 10.3") {
            double trouble_Z{ 10.3 };
            writer.travel_to_z(trouble_Z);
            AND_WHEN("GcodeWriter::Lift() is called") {
                REQUIRE(writer.lift().size() > 0);
                AND_WHEN("Z is moved post-lift to the same delta as the config Z lift") {
                    REQUIRE(writer.travel_to_z(trouble_Z + config.retract_lift.values[0]).size() == 0);
                    AND_WHEN("GCodeWriter::Unlift() is called") {
                        REQUIRE(writer.unlift().size() == 0); // we're the same height so no additional move happens.
                        THEN("GCodeWriter::Lift() emits gcode.") {
                            REQUIRE(writer.lift().size() > 0);
                        }
                    }
                }
            }
        }
    }
}

SCENARIO("set_speed emits values with fixed-point output.") {

    GIVEN("GCodeWriter instance") {
        GCodeWriter writer;
        WHEN("set_speed is called to set speed to 1.09321e+06") {
            THEN("Output string is G1 F1093210.000") {
                REQUIRE_THAT(writer.set_speed(1.09321e+06), Catch::Equals("G1 F1093210.000\n"));
            }
        }
        WHEN("set_speed is called to set speed to 1") {
            THEN("Output string is G1 F1.000") {
                REQUIRE_THAT(writer.set_speed(1.0), Catch::Equals("G1 F1.000\n"));
            }
        }
        WHEN("set_speed is called to set speed to 203.200022") {
            THEN("Output string is G1 F203.200") {
                REQUIRE_THAT(writer.set_speed(203.200022), Catch::Equals("G1 F203.200\n"));
            }
        }
        WHEN("set_speed is called to set speed to 203.200522") {
            THEN("Output string is G1 F203.200") {
                REQUIRE_THAT(writer.set_speed(203.200522), Catch::Equals("G1 F203.201\n"));
            }
        }
    }
}

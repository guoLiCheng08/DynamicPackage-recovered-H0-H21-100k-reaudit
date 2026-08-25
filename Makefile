# DynamicPackage 高保真恢复工程：严格构建，不运行原 ELF。
CC ?= gcc
AR ?= ar
CFLAGS ?= -std=c11 -O0 -g -Wall -Wextra -Wpedantic -Werror -fno-fast-math -ffp-contract=off
CPPFLAGS ?= -D_XOPEN_SOURCE=700 -Isrc

BUILD := build
# dynamic_recovered.c 是保留的早期控制流草稿，含未恢复的外部占位调用，不参与高保真库链接。
SRC := src/dynamic_math.c src/dynamic_devices.c src/dynamic_device_globals.c src/dynamic_measure_globals.c src/dynamic_mainout_globals.c src/dynamic_dyn_main_globals.c src/dynamic_core_input_globals.c src/dynamic_core_input.c src/dynamic_core_bridge.c src/dynamic_torque.c src/dynamic_orbit.c src/dynamic_time.c src/dynamic_rk4.c src/dynamic_flex.c src/dynamic_dynamics.c src/dynamic_satellite_globals.c src/dynamic_dll_init.c src/dynamic_command_wrappers.c src/dynamic_telemetry.c src/dynamic_telemetry_wrappers.c src/dynamic_scheduler.c src/dynamic_sensor_schedule.c src/dynamic_sensors.c src/dynamic_environment.c src/dynamic_core_environment.c src/dynamic_main_bridge.c src/dynamic_ipc_telemetry.c
OBJ := $(SRC:src/%.c=$(BUILD)/%.o)
LIB := $(BUILD)/libdynamicpackage_recovered.a

.PHONY: all clean selftest check-h0-fifty check-h0-hundred

all: $(LIB)

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/%.o: src/%.c | $(BUILD)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(LIB): $(OBJ)
	$(AR) rcs $@ $^

selftest: all
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/math_abi_selftest.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/math_abi_selftest
	$(BUILD)/math_abi_selftest
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/flex_rigid_selftest.c $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/flex_rigid_selftest
	$(BUILD)/flex_rigid_selftest
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/flex_effective_inertia_real_model_compare.c $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/flex_effective_inertia_real_model_compare
	$(BUILD)/flex_effective_inertia_real_model_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/vector_descriptor_nonfatal_p3_gold_compare.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/vector_descriptor_nonfatal_p3_gold_compare
	$(BUILD)/vector_descriptor_nonfatal_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/vector_memcpy_null_source_p3_death_test.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/vector_memcpy_null_source_p3_death_test
	$(BUILD)/vector_memcpy_null_source_p3_death_test
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/vector_add_nonfatal_p3_gold_compare.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/vector_add_nonfatal_p3_gold_compare
	$(BUILD)/vector_add_nonfatal_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/vector_sub_nonfatal_p3_gold_compare.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/vector_sub_nonfatal_p3_gold_compare
	$(BUILD)/vector_sub_nonfatal_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/vector_set_zero_nonfatal_p3_gold_compare.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/vector_set_zero_nonfatal_p3_gold_compare
	$(BUILD)/vector_set_zero_nonfatal_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/vector_print_p3_gold_compare.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/vector_print_p3_gold_compare
	$(BUILD)/vector_print_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/vector_abs_max_nonfatal_p3_gold_compare.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/vector_abs_max_nonfatal_p3_gold_compare
	$(BUILD)/vector_abs_max_nonfatal_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/vector_add_constant_nonfatal_p3_gold_compare.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/vector_add_constant_nonfatal_p3_gold_compare
	$(BUILD)/vector_add_constant_nonfatal_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/vector_scale_nonfatal_p3_gold_compare.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/vector_scale_nonfatal_p3_gold_compare
	$(BUILD)/vector_scale_nonfatal_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/vector_nrm2_nonfatal_p3_gold_compare.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/vector_nrm2_nonfatal_p3_gold_compare
	$(BUILD)/vector_nrm2_nonfatal_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/is_sun_in_fov_p3_gold_compare.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/is_sun_in_fov_p3_gold_compare
	$(BUILD)/is_sun_in_fov_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/is_earth_out_p3_gold_compare.c $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/is_earth_out_p3_gold_compare
	$(BUILD)/is_earth_out_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/is_sun_out_p3_gold_compare.c $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/is_sun_out_p3_gold_compare
	$(BUILD)/is_sun_out_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/ran_gaussian_p3_gold_compare.c $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/ran_gaussian_p3_gold_compare
	$(BUILD)/ran_gaussian_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/wheel_matrix_calc_p3_gold_compare.c $(BUILD)/dynamic_devices.o $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/wheel_matrix_calc_p3_gold_compare
	$(BUILD)/wheel_matrix_calc_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/vector_square_nonfatal_p3_gold_compare.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/vector_square_nonfatal_p3_gold_compare
	$(BUILD)/vector_square_nonfatal_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/vector_unit_nonfatal_p3_gold_compare.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/vector_unit_nonfatal_p3_gold_compare
	$(BUILD)/vector_unit_nonfatal_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/vector_dot_nonfatal_p3_gold_compare.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/vector_dot_nonfatal_p3_gold_compare
	$(BUILD)/vector_dot_nonfatal_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/vector3_cross_nonfatal_p3_gold_compare.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/vector3_cross_nonfatal_p3_gold_compare
	$(BUILD)/vector3_cross_nonfatal_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/quat_cpy_p3_gold_compare.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/quat_cpy_p3_gold_compare
	$(BUILD)/quat_cpy_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/quat_print_p3_gold_compare.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/quat_print_p3_gold_compare
	$(BUILD)/quat_print_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/quat_cross_p3_gold_compare.c $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/quat_cross_p3_gold_compare
	$(BUILD)/quat_cross_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/y_q_unit_p3_gold_compare.c $(LIB) -lm -o $(BUILD)/y_q_unit_p3_gold_compare
	$(BUILD)/y_q_unit_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/update_core_dyn_input_p4_gold_compare.c $(LIB) -lm -o $(BUILD)/update_core_dyn_input_p4_gold_compare
	$(BUILD)/update_core_dyn_input_p4_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dynamic_dll_init_p0_gold_compare.c $(LIB) -lm -o $(BUILD)/dynamic_dll_init_p0_gold_compare
	$(BUILD)/dynamic_dll_init_p0_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_init_array_p0_gold_compare.c $(LIB) -lm -o $(BUILD)/dyn_init_array_p0_gold_compare
	$(BUILD)/dyn_init_array_p0_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/p0_telemetry_wrappers_gold_compare.c $(LIB) -lm -o $(BUILD)/p0_telemetry_wrappers_gold_compare
	$(BUILD)/p0_telemetry_wrappers_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/p0_command_nop_abi_compare.c $(LIB) -lm -o $(BUILD)/p0_command_nop_abi_compare
	$(BUILD)/p0_command_nop_abi_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/command_telemetry_noarg_public_gold_compare.c $(LIB) -lm -o $(BUILD)/command_telemetry_noarg_public_gold_compare
	$(BUILD)/command_telemetry_noarg_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/p0_command_parse_execute_gold_compare.c $(LIB) -lm -o $(BUILD)/p0_command_parse_execute_gold_compare
	$(BUILD)/p0_command_parse_execute_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/p0_get_desk_command_gold_compare.c $(LIB) -lm -pthread -o $(BUILD)/p0_get_desk_command_gold_compare
	$(BUILD)/p0_get_desk_command_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/gravity_gradient_torque_public_gold_compare.c $(LIB) -lm -o $(BUILD)/gravity_gradient_torque_public_gold_compare
	$(BUILD)/gravity_gradient_torque_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/calc_inertial_magnetic_vector_public_gold_compare.c $(LIB) -lm -o $(BUILD)/calc_inertial_magnetic_vector_public_gold_compare
	$(BUILD)/calc_inertial_magnetic_vector_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/magnetic_vector_public_gold_compare.c $(LIB) -lm -o $(BUILD)/magnetic_vector_public_gold_compare
	$(BUILD)/magnetic_vector_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/p1_ipc_setters_abi_compare.c $(LIB) -lm -pthread -o $(BUILD)/p1_ipc_setters_abi_compare
	$(BUILD)/p1_ipc_setters_abi_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/mag_torque_public_abi_compare.c $(LIB) -lm -o $(BUILD)/mag_torque_public_abi_compare
	$(BUILD)/mag_torque_public_abi_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/rwheel_torque_public_gold_compare.c $(LIB) -lm -o $(BUILD)/rwheel_torque_public_gold_compare
	$(BUILD)/rwheel_torque_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/setsada_public_gold_compare.c $(LIB) -lm -o $(BUILD)/setsada_public_gold_compare
	$(BUILD)/setsada_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/sun_public_gold_compare.c $(LIB) -lm -o $(BUILD)/sun_public_gold_compare
	$(BUILD)/sun_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/get_wheel_speed_public_gold_compare.c $(LIB) -lm -o $(BUILD)/get_wheel_speed_public_gold_compare
	$(BUILD)/get_wheel_speed_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/set_wheel_acc_public_gold_compare.c $(LIB) -lm -o $(BUILD)/set_wheel_acc_public_gold_compare
	$(BUILD)/set_wheel_acc_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/update_mag_moment_public_gold_compare.c $(LIB) -lm -o $(BUILD)/update_mag_moment_public_gold_compare
	$(BUILD)/update_mag_moment_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/update_wheel_public_gold_compare.c $(LIB) -lm -o $(BUILD)/update_wheel_public_gold_compare
	$(BUILD)/update_wheel_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/drive_sada_once_public_gold_compare.c $(LIB) -lm -o $(BUILD)/drive_sada_once_public_gold_compare
	$(BUILD)/drive_sada_once_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/get_sada_angle_public_gold_compare.c $(LIB) -lm -o $(BUILD)/get_sada_angle_public_gold_compare
	$(BUILD)/get_sada_angle_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/set_moment_mtq_public_gold_compare.c $(LIB) -lm -o $(BUILD)/set_moment_mtq_public_gold_compare
	$(BUILD)/set_moment_mtq_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/set_thruster_work_status_public_gold_compare.c $(LIB) -lm -o $(BUILD)/set_thruster_work_status_public_gold_compare
	$(BUILD)/set_thruster_work_status_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/update_thruster_public_gold_compare.c $(LIB) -lm -o $(BUILD)/update_thruster_public_gold_compare
	$(BUILD)/update_thruster_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/is_earth_shadow_public_gold_compare.c $(LIB) -lm -o $(BUILD)/is_earth_shadow_public_gold_compare
	$(BUILD)/is_earth_shadow_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/get_star_tracker_quat_public_gold_compare.c $(LIB) -lm -o $(BUILD)/get_star_tracker_quat_public_gold_compare
	$(BUILD)/get_star_tracker_quat_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/is_star_tracker_valid_public_gold_compare.c $(LIB) -lm -o $(BUILD)/is_star_tracker_valid_public_gold_compare
	$(BUILD)/is_star_tracker_valid_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/update_sts_quat_public_gold_compare.c $(LIB) -lm -o $(BUILD)/update_sts_quat_public_gold_compare
	$(BUILD)/update_sts_quat_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/update_gps_public_gold_compare.c $(LIB) -lm -o $(BUILD)/update_gps_public_gold_compare
	$(BUILD)/update_gps_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/update_sun_sensor_valid_flag_public_gold_compare.c $(LIB) -lm -o $(BUILD)/update_sun_sensor_valid_flag_public_gold_compare
	$(BUILD)/update_sun_sensor_valid_flag_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/update_dss_public_gold_compare.c $(LIB) -lm -o $(BUILD)/update_dss_public_gold_compare
	$(BUILD)/update_dss_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/update_gyro_public_gold_compare.c $(LIB) -lm -o $(BUILD)/update_gyro_public_gold_compare
	$(BUILD)/update_gyro_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/update_magmeter_public_gold_compare.c $(LIB) -lm -o $(BUILD)/update_magmeter_public_gold_compare
	$(BUILD)/update_magmeter_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/update_sts_valid_flag_public_gold_compare.c $(LIB) -lm -o $(BUILD)/update_sts_valid_flag_public_gold_compare
	$(BUILD)/update_sts_valid_flag_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/send_dyn_tele_public_gold_compare.c $(LIB) -lm -pthread -lrt -o $(BUILD)/send_dyn_tele_public_gold_compare
	$(BUILD)/send_dyn_tele_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/update_sat_inertia_xw_public_gold_compare.c $(LIB) -lm -o $(BUILD)/update_sat_inertia_xw_public_gold_compare
	$(BUILD)/update_sat_inertia_xw_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/update_sat_inertia_public_gold_compare.c $(LIB) -lm -o $(BUILD)/update_sat_inertia_public_gold_compare
	$(BUILD)/update_sat_inertia_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/intergrator_init_public_gold_compare.c $(LIB) -lm -o $(BUILD)/intergrator_init_public_gold_compare
	$(BUILD)/intergrator_init_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/intergrator_show_public_gold_compare.c $(LIB) -lm -o $(BUILD)/intergrator_show_public_gold_compare
	$(BUILD)/intergrator_show_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/localgeo_public_gold_compare.c $(LIB) -lm -o $(BUILD)/localgeo_public_gold_compare
	$(BUILD)/localgeo_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dynamics_public_gold_compare.c $(LIB) -lm -o $(BUILD)/dynamics_public_gold_compare
	$(BUILD)/dynamics_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/getdyninput_public_gold_compare.c $(LIB) -lm -pthread -lrt -o $(BUILD)/getdyninput_public_gold_compare
	$(BUILD)/getdyninput_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/gh_change_public_gold_compare.c $(LIB) -lm -o $(BUILD)/gh_change_public_gold_compare
	$(BUILD)/gh_change_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/pnm_public_gold_compare.c $(LIB) -lm -o $(BUILD)/pnm_public_gold_compare
	$(BUILD)/pnm_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/frontalarea_public_gold_compare.c $(LIB) -lm -o $(BUILD)/frontalarea_public_gold_compare
	$(BUILD)/frontalarea_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/aerodynamic_torque_public_gold_compare.c $(LIB) -lm -o $(BUILD)/aerodynamic_torque_public_gold_compare
	$(BUILD)/aerodynamic_torque_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_array_public_gold_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_array_public_gold_compare
	$(BUILD)/dyn_main_array_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/quat_conjugate_public_gold_compare.c $(LIB) -lm -o $(BUILD)/quat_conjugate_public_gold_compare
	$(BUILD)/quat_conjugate_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/quat_norm_public_gold_compare.c $(LIB) -lm -o $(BUILD)/quat_norm_public_gold_compare
	$(BUILD)/quat_norm_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/quat_xi_public_gold_compare.c $(LIB) -lm -o $(BUILD)/quat_xi_public_gold_compare
	$(BUILD)/quat_xi_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/var2buff_public_gold_compare.c $(LIB) -lm -o $(BUILD)/var2buff_public_gold_compare
	$(BUILD)/var2buff_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/update_satellite_public_gold_compare.c $(LIB) -lm -o $(BUILD)/update_satellite_public_gold_compare
	$(BUILD)/update_satellite_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/put_buffers_public_gold_compare.c $(LIB) -lm -o $(BUILD)/put_buffers_public_gold_compare
	$(BUILD)/put_buffers_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/get_para_buffers_public_gold_compare.c $(LIB) -lm -o $(BUILD)/get_para_buffers_public_gold_compare
	$(BUILD)/get_para_buffers_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/rotation_yz_public_gold_compare.c $(LIB) -lm -o $(BUILD)/rotation_yz_public_gold_compare
	$(BUILD)/rotation_yz_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/torque_moment_public_gold_compare.c $(LIB) -lm -o $(BUILD)/torque_moment_public_gold_compare
	$(BUILD)/torque_moment_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_init_get_public_gold_compare.c $(LIB) -lm -o $(BUILD)/time_init_get_public_gold_compare
	$(BUILD)/time_init_get_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dynamics_flex_runtime_replay_compare.c $(LIB) -lm -o $(BUILD)/dynamics_flex_runtime_replay_compare
	$(BUILD)/dynamics_flex_runtime_replay_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/device_initializers_public_gold_compare.c $(LIB) -lm -o $(BUILD)/device_initializers_public_gold_compare
	$(BUILD)/device_initializers_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/thruster_init_public_gold_compare.c $(LIB) -lm -o $(BUILD)/thruster_init_public_gold_compare
	$(BUILD)/thruster_init_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/wheel_init_public_gold_compare.c $(LIB) -lm -o $(BUILD)/wheel_init_public_gold_compare
	$(BUILD)/wheel_init_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/sts_init_public_gold_compare.c $(LIB) -lm -o $(BUILD)/sts_init_public_gold_compare
	$(BUILD)/sts_init_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/magtorque_init_public_gold_compare.c $(LIB) -lm -o $(BUILD)/magtorque_init_public_gold_compare
	$(BUILD)/magtorque_init_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/get_wheel_moment_public_gold_compare.c $(LIB) -lm -o $(BUILD)/get_wheel_moment_public_gold_compare
	$(BUILD)/get_wheel_moment_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/set_wheel_hbdot_public_gold_compare.c $(LIB) -lm -o $(BUILD)/set_wheel_hbdot_public_gold_compare
	$(BUILD)/set_wheel_hbdot_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/torque_wheel_moment_short_abi_gold_compare.c $(LIB) -lm -o $(BUILD)/torque_wheel_moment_short_abi_gold_compare
	$(BUILD)/torque_wheel_moment_short_abi_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/julian_centuries_public_gold_compare.c $(LIB) -lm -o $(BUILD)/julian_centuries_public_gold_compare
	$(BUILD)/julian_centuries_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/get_sun_vector_public_gold_compare.c $(LIB) -lm -o $(BUILD)/get_sun_vector_public_gold_compare
	$(BUILD)/get_sun_vector_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/attitude_p1_public_gold_compare.c $(LIB) -lm -o $(BUILD)/attitude_p1_public_gold_compare
	$(BUILD)/attitude_p1_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/get_orbit_rv_public_gold_compare.c $(LIB) -lm -o $(BUILD)/get_orbit_rv_public_gold_compare
	$(BUILD)/get_orbit_rv_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/set_inertia_tensor_public_gold_compare.c $(LIB) -lm -o $(BUILD)/set_inertia_tensor_public_gold_compare
	$(BUILD)/set_inertia_tensor_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/set_spacecraft_mass_public_gold_compare.c $(LIB) -lm -o $(BUILD)/set_spacecraft_mass_public_gold_compare
	$(BUILD)/set_spacecraft_mass_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/set_sat_inertia_tensor_public_gold_compare.c $(LIB) -lm -o $(BUILD)/set_sat_inertia_tensor_public_gold_compare
	$(BUILD)/set_sat_inertia_tensor_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/torque_init_public_gold_compare.c $(LIB) -lm -o $(BUILD)/torque_init_public_gold_compare
	$(BUILD)/torque_init_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/update_external_force_public_gold_compare.c $(LIB) -lm -o $(BUILD)/update_external_force_public_gold_compare
	$(BUILD)/update_external_force_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/quat_psi_public_gold_compare.c $(LIB) -lm -o $(BUILD)/quat_psi_public_gold_compare
	$(BUILD)/quat_psi_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/get_inertial_mag_public_gold_compare.c $(LIB) -lm -o $(BUILD)/get_inertial_mag_public_gold_compare
	$(BUILD)/get_inertial_mag_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_init_public_gold_compare.c $(LIB) -lm -o $(BUILD)/dyn_init_public_gold_compare
	$(BUILD)/dyn_init_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/update_torque_public_gold_compare.c $(LIB) -lm -o $(BUILD)/update_torque_public_gold_compare
	$(BUILD)/update_torque_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/mag_update_public_gold_compare.c $(LIB) -lm -o $(BUILD)/mag_update_public_gold_compare
	$(BUILD)/mag_update_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/sat_para_init_public_gold_compare.c $(LIB) -lm -o $(BUILD)/sat_para_init_public_gold_compare
	$(BUILD)/sat_para_init_public_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/array_helpers_p3_gold_compare.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/array_helpers_p3_gold_compare
	$(BUILD)/array_helpers_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/frac_rng_p3_gold_compare.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/frac_rng_p3_gold_compare
	$(BUILD)/frac_rng_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/c2_s2_p3_gold_compare.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/c2_s2_p3_gold_compare
	$(BUILD)/c2_s2_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/eccentric2true_p3_gold_compare.c $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/eccentric2true_p3_gold_compare
	$(BUILD)/eccentric2true_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/eccentric2mean_p3_gold_compare.c $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/eccentric2mean_p3_gold_compare
	$(BUILD)/eccentric2mean_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/niceangle_p3_gold_compare.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/niceangle_p3_gold_compare
	$(BUILD)/niceangle_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/rotation_xyz_p3_gold_compare.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/rotation_xyz_p3_gold_compare
	$(BUILD)/rotation_xyz_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/protocol_put_p3_gold_compare.c $(BUILD)/dynamic_ipc_telemetry.o -lm -o $(BUILD)/protocol_put_p3_gold_compare
	$(BUILD)/protocol_put_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/protocol_read_p3_gold_compare.c $(BUILD)/dynamic_ipc_telemetry.o -lm -o $(BUILD)/protocol_read_p3_gold_compare
	$(BUILD)/protocol_read_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/change_endian_p3_gold_compare.c $(BUILD)/dynamic_ipc_telemetry.o -lm -o $(BUILD)/change_endian_p3_gold_compare
	$(BUILD)/change_endian_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/protocol_para_p3_gold_compare.c $(BUILD)/dynamic_ipc_telemetry.o -lm -o $(BUILD)/protocol_para_p3_gold_compare
	$(BUILD)/protocol_para_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/shared_ipc_getters_p2_gold_compare.c $(BUILD)/dynamic_ipc_telemetry.o -pthread -lm -o $(BUILD)/shared_ipc_getters_p2_gold_compare
	$(BUILD)/shared_ipc_getters_p2_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/shared_ipc_getters_boundary_p3_gold_compare.c $(BUILD)/dynamic_ipc_telemetry.o -pthread -lm -o $(BUILD)/shared_ipc_getters_boundary_p3_gold_compare
	$(BUILD)/shared_ipc_getters_boundary_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/axis_vector2matrix_p3_gold_compare.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/axis_vector2matrix_p3_gold_compare
	$(BUILD)/axis_vector2matrix_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/vector3_to_matrix_p3_gold_compare.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/vector3_to_matrix_p3_gold_compare
	$(BUILD)/vector3_to_matrix_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/euler_312_to_quat_p3_gold_compare.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/euler_312_to_quat_p3_gold_compare
	$(BUILD)/euler_312_to_quat_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/quat_to_euler_312_p3_death_test.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/quat_to_euler_312_p3_death_test
	$(BUILD)/quat_to_euler_312_p3_death_test
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/quat2matrix_p3_death_test.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/quat2matrix_p3_death_test
	$(BUILD)/quat2matrix_p3_death_test
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/cartesian2polar_p3_gold_compare.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/cartesian2polar_p3_gold_compare
	$(BUILD)/cartesian2polar_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/escape_protocol_p3_gold_compare.c $(BUILD)/dynamic_ipc_telemetry.o -lm -o $(BUILD)/escape_protocol_p3_gold_compare
	$(BUILD)/escape_protocol_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/matrix2quat_p3_gold_compare.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/matrix2quat_p3_gold_compare
	$(BUILD)/matrix2quat_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/vector_axpy_nonfatal_p3_gold_compare.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/vector_axpy_nonfatal_p3_gold_compare
	$(BUILD)/vector_axpy_nonfatal_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/vector_axpy_null_source_p3_death_test.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/vector_axpy_null_source_p3_death_test
	$(BUILD)/vector_axpy_null_source_p3_death_test
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/vector_axpby_nonfatal_p3_gold_compare.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/vector_axpby_nonfatal_p3_gold_compare
	$(BUILD)/vector_axpby_nonfatal_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/vector_axpby_null_source_p3_death_test.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/vector_axpby_null_source_p3_death_test
	$(BUILD)/vector_axpby_null_source_p3_death_test
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/matrix_descriptor_nonfatal_p3_gold_compare.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/matrix_descriptor_nonfatal_p3_gold_compare
	$(BUILD)/matrix_descriptor_nonfatal_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/inv_cal_m3_p3_gold_compare.c $(LIB) -lm -o $(BUILD)/inv_cal_m3_p3_gold_compare
	$(BUILD)/inv_cal_m3_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/matrix_trans_nonfatal_p3_gold_compare.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/matrix_trans_nonfatal_p3_gold_compare
	$(BUILD)/matrix_trans_nonfatal_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/matrix_set_zero_nonfatal_p3_gold_compare.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/matrix_set_zero_nonfatal_p3_gold_compare
	$(BUILD)/matrix_set_zero_nonfatal_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/matrix_set_all_nonfatal_p3_gold_compare.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/matrix_set_all_nonfatal_p3_gold_compare
	$(BUILD)/matrix_set_all_nonfatal_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/matrix_set_col_nonfatal_p3_gold_compare.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/matrix_set_col_nonfatal_p3_gold_compare
	$(BUILD)/matrix_set_col_nonfatal_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/matrix_print_p3_gold_compare.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/matrix_print_p3_gold_compare
	$(BUILD)/matrix_print_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/matrix_add_constant_nonfatal_p3_gold_compare.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/matrix_add_constant_nonfatal_p3_gold_compare
	$(BUILD)/matrix_add_constant_nonfatal_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/matrix_scale_nonfatal_p3_gold_compare.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/matrix_scale_nonfatal_p3_gold_compare
	$(BUILD)/matrix_scale_nonfatal_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/matrix_add_nonfatal_p3_gold_compare.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/matrix_add_nonfatal_p3_gold_compare
	$(BUILD)/matrix_add_nonfatal_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/matrix_sub_nonfatal_p3_gold_compare.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/matrix_sub_nonfatal_p3_gold_compare
	$(BUILD)/matrix_sub_nonfatal_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/matrix_mul_elements_nonfatal_p3_gold_compare.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/matrix_mul_elements_nonfatal_p3_gold_compare
	$(BUILD)/matrix_mul_elements_nonfatal_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/matrix_div_elements_nonfatal_p3_gold_compare.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/matrix_div_elements_nonfatal_p3_gold_compare
	$(BUILD)/matrix_div_elements_nonfatal_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/blas_gemv_descriptor_nonfatal_p3_gold_compare.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/blas_gemv_descriptor_nonfatal_p3_gold_compare
	$(BUILD)/blas_gemv_descriptor_nonfatal_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/blas_gemm_nonfatal_p3_gold_compare.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/blas_gemm_nonfatal_p3_gold_compare
	$(BUILD)/blas_gemm_nonfatal_p3_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/flex_modal_real_model_compare.c $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/flex_modal_real_model_compare
	$(BUILD)/flex_modal_real_model_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/update_gps_init_flag_p2_gold_compare.c $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/update_gps_init_flag_p2_gold_compare
	$(BUILD)/update_gps_init_flag_p2_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dss_shadow_boundary_p2_gold_compare.c $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/dss_shadow_boundary_p2_gold_compare
	$(BUILD)/dss_shadow_boundary_p2_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/differential_equation_33_gold_compare.c $(BUILD)/dynamic_dynamics.o $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/differential_equation_33_gold_compare
	$(BUILD)/differential_equation_33_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/orbit_dynamic_high_ecc_shadow_gold_compare.c $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/orbit_dynamic_high_ecc_shadow_gold_compare
	$(BUILD)/orbit_dynamic_high_ecc_shadow_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/differential_equation_global_original_gold_compare.c $(BUILD)/dynamic_satellite_globals.o $(BUILD)/dynamic_torque.o $(BUILD)/dynamic_core_bridge.o $(BUILD)/dynamic_core_environment.o $(BUILD)/dynamic_environment.o $(BUILD)/dynamic_dynamics.o $(BUILD)/dynamic_rk4.o $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_devices.o $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/differential_equation_global_original_gold_compare
	$(BUILD)/differential_equation_global_original_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/rk4_integrator_global_original_gold_compare.c $(BUILD)/dynamic_satellite_globals.o $(BUILD)/dynamic_torque.o $(BUILD)/dynamic_core_bridge.o $(BUILD)/dynamic_core_environment.o $(BUILD)/dynamic_environment.o $(BUILD)/dynamic_dynamics.o $(BUILD)/dynamic_rk4.o $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_devices.o $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/rk4_integrator_global_original_gold_compare
	$(BUILD)/rk4_integrator_global_original_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) -DGOLD=\"analysis/golden/dynamic_init_global_abi/\" analysis/dynamic_init_global_original_gold_compare.c $(BUILD)/dynamic_satellite_globals.o $(BUILD)/dynamic_core_bridge.o $(BUILD)/dynamic_core_environment.o $(BUILD)/dynamic_dynamics.o $(BUILD)/dynamic_rk4.o $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_environment.o $(BUILD)/dynamic_math.o $(BUILD)/dynamic_torque.o -lm -o $(BUILD)/dynamic_init_global_default_gold_compare
	$(BUILD)/dynamic_init_global_default_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) -DGOLD=\"analysis/golden/dynamic_init_global_abi/controlled_input/\" analysis/dynamic_init_global_original_gold_compare.c $(BUILD)/dynamic_satellite_globals.o $(BUILD)/dynamic_core_bridge.o $(BUILD)/dynamic_core_environment.o $(BUILD)/dynamic_dynamics.o $(BUILD)/dynamic_rk4.o $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_environment.o $(BUILD)/dynamic_math.o $(BUILD)/dynamic_torque.o -lm -o $(BUILD)/dynamic_init_global_controlled_gold_compare
	$(BUILD)/dynamic_init_global_controlled_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) -DGOLD=\"analysis/golden/update_device_control_global_abi/controlled_input/\" analysis/update_device_control_global_original_gold_compare.c $(BUILD)/dynamic_devices.o $(BUILD)/dynamic_device_globals.o $(BUILD)/dynamic_satellite_globals.o $(BUILD)/dynamic_core_bridge.o $(BUILD)/dynamic_core_environment.o $(BUILD)/dynamic_dynamics.o $(BUILD)/dynamic_rk4.o $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_environment.o $(BUILD)/dynamic_math.o $(BUILD)/dynamic_torque.o $(BUILD)/dynamic_sensors.o -lm -o $(BUILD)/update_device_control_global_flag1_gold_compare
	$(BUILD)/update_device_control_global_flag1_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) -DGOLD=\"analysis/golden/update_device_control_global_abi/flag0_input/\" analysis/update_device_control_global_original_gold_compare.c $(BUILD)/dynamic_devices.o $(BUILD)/dynamic_device_globals.o $(BUILD)/dynamic_satellite_globals.o $(BUILD)/dynamic_core_bridge.o $(BUILD)/dynamic_core_environment.o $(BUILD)/dynamic_dynamics.o $(BUILD)/dynamic_rk4.o $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_environment.o $(BUILD)/dynamic_math.o $(BUILD)/dynamic_torque.o $(BUILD)/dynamic_sensors.o -lm -o $(BUILD)/update_device_control_global_flag0_gold_compare
	$(BUILD)/update_device_control_global_flag0_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/update_device_measure_global_original_gold_compare.c $(BUILD)/dynamic_measure_globals.o $(BUILD)/dynamic_satellite_globals.o $(BUILD)/dynamic_torque.o $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_environment.o $(BUILD)/dynamic_core_environment.o $(BUILD)/dynamic_math.o $(BUILD)/dynamic_core_bridge.o $(BUILD)/dynamic_dynamics.o $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_rk4.o -lm -o $(BUILD)/update_device_measure_global_gold_compare
	$(BUILD)/update_device_measure_global_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/update_device_measure_gps_init_flag_p2_gold_compare.c $(BUILD)/dynamic_measure_globals.o $(BUILD)/dynamic_satellite_globals.o $(BUILD)/dynamic_torque.o $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_environment.o $(BUILD)/dynamic_core_environment.o $(BUILD)/dynamic_math.o $(BUILD)/dynamic_core_bridge.o $(BUILD)/dynamic_dynamics.o $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_rk4.o -lm -o $(BUILD)/update_device_measure_gps_init_flag_p2_gold_compare
	$(BUILD)/update_device_measure_gps_init_flag_p2_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/update_mainout_global_original_gold_compare.c $(BUILD)/dynamic_mainout_globals.o $(BUILD)/dynamic_measure_globals.o $(BUILD)/dynamic_device_globals.o $(BUILD)/dynamic_telemetry.o $(BUILD)/dynamic_devices.o $(BUILD)/dynamic_satellite_globals.o $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_environment.o $(BUILD)/dynamic_core_environment.o $(BUILD)/dynamic_core_bridge.o $(BUILD)/dynamic_dynamics.o $(BUILD)/dynamic_rk4.o $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_math.o $(BUILD)/dynamic_torque.o -lm -o $(BUILD)/update_mainout_global_deterministic_compare
	$(BUILD)/update_mainout_global_deterministic_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/update_mainout_global_seeded_sts_gold_compare.c $(BUILD)/dynamic_mainout_globals.o $(BUILD)/dynamic_measure_globals.o $(BUILD)/dynamic_device_globals.o $(BUILD)/dynamic_telemetry.o $(BUILD)/dynamic_devices.o $(BUILD)/dynamic_satellite_globals.o $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_environment.o $(BUILD)/dynamic_core_environment.o $(BUILD)/dynamic_core_bridge.o $(BUILD)/dynamic_dynamics.o $(BUILD)/dynamic_rk4.o $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_math.o $(BUILD)/dynamic_torque.o -lm -o $(BUILD)/update_mainout_global_seeded_sts_compare
	$(BUILD)/update_mainout_global_seeded_sts_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/update_mainout_sts_switch_p2_gold_compare.c $(BUILD)/dynamic_mainout_globals.o $(BUILD)/dynamic_measure_globals.o $(BUILD)/dynamic_device_globals.o $(BUILD)/dynamic_telemetry.o $(BUILD)/dynamic_devices.o $(BUILD)/dynamic_satellite_globals.o $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_environment.o $(BUILD)/dynamic_core_environment.o $(BUILD)/dynamic_core_bridge.o $(BUILD)/dynamic_dynamics.o $(BUILD)/dynamic_rk4.o $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_math.o $(BUILD)/dynamic_torque.o -lm -o $(BUILD)/update_mainout_sts_switch_p2_gold_compare
	$(BUILD)/update_mainout_sts_switch_p2_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_step1_original_gold_compare.c $(BUILD)/dynamic_dyn_main_globals.o $(BUILD)/dynamic_core_input_globals.o $(BUILD)/dynamic_core_input.o $(BUILD)/dynamic_mainout_globals.o $(BUILD)/dynamic_measure_globals.o $(BUILD)/dynamic_device_globals.o $(BUILD)/dynamic_telemetry.o $(BUILD)/dynamic_ipc_telemetry.o $(BUILD)/dynamic_devices.o $(BUILD)/dynamic_satellite_globals.o $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_environment.o $(BUILD)/dynamic_core_environment.o $(BUILD)/dynamic_core_bridge.o $(BUILD)/dynamic_dynamics.o $(BUILD)/dynamic_rk4.o $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_math.o $(BUILD)/dynamic_torque.o -lm -o $(BUILD)/dyn_main_global_two_step_ipc_gold_compare
	$(BUILD)/dyn_main_global_two_step_ipc_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_rw1_pos_two_step_compare.c $(BUILD)/dynamic_dyn_main_globals.o $(BUILD)/dynamic_core_input_globals.o $(BUILD)/dynamic_core_input.o $(BUILD)/dynamic_mainout_globals.o $(BUILD)/dynamic_measure_globals.o $(BUILD)/dynamic_device_globals.o $(BUILD)/dynamic_telemetry.o $(BUILD)/dynamic_ipc_telemetry.o $(BUILD)/dynamic_devices.o $(BUILD)/dynamic_satellite_globals.o $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_environment.o $(BUILD)/dynamic_core_environment.o $(BUILD)/dynamic_core_bridge.o $(BUILD)/dynamic_dynamics.o $(BUILD)/dynamic_rk4.o $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_math.o $(BUILD)/dynamic_torque.o -lm -o $(BUILD)/dyn_main_global_rw1_pos_two_step_ipc_gold_compare
	$(BUILD)/dyn_main_global_rw1_pos_two_step_ipc_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_mtq_x_pos_two_step_compare.c $(BUILD)/dynamic_dyn_main_globals.o $(BUILD)/dynamic_core_input_globals.o $(BUILD)/dynamic_core_input.o $(BUILD)/dynamic_mainout_globals.o $(BUILD)/dynamic_measure_globals.o $(BUILD)/dynamic_device_globals.o $(BUILD)/dynamic_telemetry.o $(BUILD)/dynamic_ipc_telemetry.o $(BUILD)/dynamic_devices.o $(BUILD)/dynamic_satellite_globals.o $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_environment.o $(BUILD)/dynamic_core_environment.o $(BUILD)/dynamic_core_bridge.o $(BUILD)/dynamic_dynamics.o $(BUILD)/dynamic_rk4.o $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_math.o $(BUILD)/dynamic_torque.o -lm -o $(BUILD)/dyn_main_global_mtq_x_pos_two_step_ipc_gold_compare
	$(BUILD)/dyn_main_global_mtq_x_pos_two_step_ipc_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_sada_position_two_step_compare.c $(BUILD)/dynamic_dyn_main_globals.o $(BUILD)/dynamic_core_input_globals.o $(BUILD)/dynamic_core_input.o $(BUILD)/dynamic_mainout_globals.o $(BUILD)/dynamic_measure_globals.o $(BUILD)/dynamic_device_globals.o $(BUILD)/dynamic_telemetry.o $(BUILD)/dynamic_ipc_telemetry.o $(BUILD)/dynamic_devices.o $(BUILD)/dynamic_satellite_globals.o $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_environment.o $(BUILD)/dynamic_core_environment.o $(BUILD)/dynamic_core_bridge.o $(BUILD)/dynamic_dynamics.o $(BUILD)/dynamic_rk4.o $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_math.o $(BUILD)/dynamic_torque.o -lm -o $(BUILD)/dyn_main_global_sada_position_two_step_ipc_gold_compare
	$(BUILD)/dyn_main_global_sada_position_two_step_ipc_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_thruster_on_two_step_compare.c $(BUILD)/dynamic_dyn_main_globals.o $(BUILD)/dynamic_core_input_globals.o $(BUILD)/dynamic_core_input.o $(BUILD)/dynamic_mainout_globals.o $(BUILD)/dynamic_measure_globals.o $(BUILD)/dynamic_device_globals.o $(BUILD)/dynamic_telemetry.o $(BUILD)/dynamic_ipc_telemetry.o $(BUILD)/dynamic_devices.o $(BUILD)/dynamic_satellite_globals.o $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_environment.o $(BUILD)/dynamic_core_environment.o $(BUILD)/dynamic_core_bridge.o $(BUILD)/dynamic_dynamics.o $(BUILD)/dynamic_rk4.o $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_math.o $(BUILD)/dynamic_torque.o -lm -o $(BUILD)/dyn_main_global_thruster_on_two_step_ipc_gold_compare
	$(BUILD)/dyn_main_global_thruster_on_two_step_ipc_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_inertia_flag1_two_step_compare.c $(BUILD)/dynamic_dyn_main_globals.o $(BUILD)/dynamic_core_input_globals.o $(BUILD)/dynamic_core_input.o $(BUILD)/dynamic_mainout_globals.o $(BUILD)/dynamic_measure_globals.o $(BUILD)/dynamic_device_globals.o $(BUILD)/dynamic_telemetry.o $(BUILD)/dynamic_ipc_telemetry.o $(BUILD)/dynamic_devices.o $(BUILD)/dynamic_satellite_globals.o $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_environment.o $(BUILD)/dynamic_core_environment.o $(BUILD)/dynamic_core_bridge.o $(BUILD)/dynamic_dynamics.o $(BUILD)/dynamic_rk4.o $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_math.o $(BUILD)/dynamic_torque.o -lm -o $(BUILD)/dyn_main_global_inertia_flag1_two_step_ipc_gold_compare
	$(BUILD)/dyn_main_global_inertia_flag1_two_step_ipc_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_inertia_flag1_thruster_on_two_step_compare.c $(BUILD)/dynamic_dyn_main_globals.o $(BUILD)/dynamic_core_input_globals.o $(BUILD)/dynamic_core_input.o $(BUILD)/dynamic_mainout_globals.o $(BUILD)/dynamic_measure_globals.o $(BUILD)/dynamic_device_globals.o $(BUILD)/dynamic_telemetry.o $(BUILD)/dynamic_ipc_telemetry.o $(BUILD)/dynamic_devices.o $(BUILD)/dynamic_satellite_globals.o $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_environment.o $(BUILD)/dynamic_core_environment.o $(BUILD)/dynamic_core_bridge.o $(BUILD)/dynamic_dynamics.o $(BUILD)/dynamic_rk4.o $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_math.o $(BUILD)/dynamic_torque.o -lm -o $(BUILD)/dyn_main_global_inertia_flag1_thruster_on_two_step_ipc_gold_compare
	$(BUILD)/dyn_main_global_inertia_flag1_thruster_on_two_step_ipc_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_zero_ten_step_compare.c $(BUILD)/dynamic_dyn_main_globals.o $(BUILD)/dynamic_core_input_globals.o $(BUILD)/dynamic_core_input.o $(BUILD)/dynamic_mainout_globals.o $(BUILD)/dynamic_measure_globals.o $(BUILD)/dynamic_device_globals.o $(BUILD)/dynamic_telemetry.o $(BUILD)/dynamic_ipc_telemetry.o $(BUILD)/dynamic_devices.o $(BUILD)/dynamic_satellite_globals.o $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_environment.o $(BUILD)/dynamic_core_environment.o $(BUILD)/dynamic_core_bridge.o $(BUILD)/dynamic_dynamics.o $(BUILD)/dynamic_rk4.o $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_math.o $(BUILD)/dynamic_torque.o -lm -o $(BUILD)/dyn_main_global_zero_ten_step_gold_compare
	$(BUILD)/dyn_main_global_zero_ten_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_mixed_ten_step_compare.c $(BUILD)/dynamic_dyn_main_globals.o $(BUILD)/dynamic_core_input_globals.o $(BUILD)/dynamic_core_input.o $(BUILD)/dynamic_mainout_globals.o $(BUILD)/dynamic_measure_globals.o $(BUILD)/dynamic_device_globals.o $(BUILD)/dynamic_telemetry.o $(BUILD)/dynamic_ipc_telemetry.o $(BUILD)/dynamic_devices.o $(BUILD)/dynamic_satellite_globals.o $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_environment.o $(BUILD)/dynamic_core_environment.o $(BUILD)/dynamic_core_bridge.o $(BUILD)/dynamic_dynamics.o $(BUILD)/dynamic_rk4.o $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_math.o $(BUILD)/dynamic_torque.o -lm -o $(BUILD)/dyn_main_global_mixed_ten_step_gold_compare
	$(BUILD)/dyn_main_global_mixed_ten_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_zero_ten_step_seed12345_compare.c $(BUILD)/dynamic_dyn_main_globals.o $(BUILD)/dynamic_core_input_globals.o $(BUILD)/dynamic_core_input.o $(BUILD)/dynamic_mainout_globals.o $(BUILD)/dynamic_measure_globals.o $(BUILD)/dynamic_device_globals.o $(BUILD)/dynamic_telemetry.o $(BUILD)/dynamic_ipc_telemetry.o $(BUILD)/dynamic_devices.o $(BUILD)/dynamic_satellite_globals.o $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_environment.o $(BUILD)/dynamic_core_environment.o $(BUILD)/dynamic_core_bridge.o $(BUILD)/dynamic_dynamics.o $(BUILD)/dynamic_rk4.o $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_math.o $(BUILD)/dynamic_torque.o -lm -o $(BUILD)/dyn_main_global_zero_ten_step_seed12345_gold_compare
	$(BUILD)/dyn_main_global_zero_ten_step_seed12345_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_sada_saturation_ten_step_compare.c $(BUILD)/dynamic_dyn_main_globals.o $(BUILD)/dynamic_core_input_globals.o $(BUILD)/dynamic_core_input.o $(BUILD)/dynamic_mainout_globals.o $(BUILD)/dynamic_measure_globals.o $(BUILD)/dynamic_device_globals.o $(BUILD)/dynamic_telemetry.o $(BUILD)/dynamic_ipc_telemetry.o $(BUILD)/dynamic_devices.o $(BUILD)/dynamic_satellite_globals.o $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_environment.o $(BUILD)/dynamic_core_environment.o $(BUILD)/dynamic_core_bridge.o $(BUILD)/dynamic_dynamics.o $(BUILD)/dynamic_rk4.o $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_math.o $(BUILD)/dynamic_torque.o -lm -o $(BUILD)/dyn_main_global_sada_saturation_ten_step_gold_compare
	$(BUILD)/dyn_main_global_sada_saturation_ten_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_mixed_twenty_step_compare.c $(BUILD)/dynamic_dyn_main_globals.o $(BUILD)/dynamic_core_input_globals.o $(BUILD)/dynamic_core_input.o $(BUILD)/dynamic_mainout_globals.o $(BUILD)/dynamic_measure_globals.o $(BUILD)/dynamic_device_globals.o $(BUILD)/dynamic_telemetry.o $(BUILD)/dynamic_ipc_telemetry.o $(BUILD)/dynamic_devices.o $(BUILD)/dynamic_satellite_globals.o $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_environment.o $(BUILD)/dynamic_core_environment.o $(BUILD)/dynamic_core_bridge.o $(BUILD)/dynamic_dynamics.o $(BUILD)/dynamic_rk4.o $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_math.o $(BUILD)/dynamic_torque.o -lm -o $(BUILD)/dyn_main_global_mixed_twenty_step_gold_compare
	$(BUILD)/dyn_main_global_mixed_twenty_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_noncollinear_mtq_two_step_compare.c $(BUILD)/dynamic_dyn_main_globals.o $(BUILD)/dynamic_core_input_globals.o $(BUILD)/dynamic_core_input.o $(BUILD)/dynamic_mainout_globals.o $(BUILD)/dynamic_measure_globals.o $(BUILD)/dynamic_device_globals.o $(BUILD)/dynamic_telemetry.o $(BUILD)/dynamic_ipc_telemetry.o $(BUILD)/dynamic_devices.o $(BUILD)/dynamic_satellite_globals.o $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_environment.o $(BUILD)/dynamic_core_environment.o $(BUILD)/dynamic_core_bridge.o $(BUILD)/dynamic_dynamics.o $(BUILD)/dynamic_rk4.o $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_math.o $(BUILD)/dynamic_torque.o -lm -o $(BUILD)/dyn_main_global_noncollinear_mtq_two_step_gold_compare
	$(BUILD)/dyn_main_global_noncollinear_mtq_two_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_noncollinear_sada_flag1_two_step_compare.c $(BUILD)/dynamic_dyn_main_globals.o $(BUILD)/dynamic_core_input_globals.o $(BUILD)/dynamic_core_input.o $(BUILD)/dynamic_mainout_globals.o $(BUILD)/dynamic_measure_globals.o $(BUILD)/dynamic_device_globals.o $(BUILD)/dynamic_telemetry.o $(BUILD)/dynamic_ipc_telemetry.o $(BUILD)/dynamic_devices.o $(BUILD)/dynamic_satellite_globals.o $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_environment.o $(BUILD)/dynamic_core_environment.o $(BUILD)/dynamic_core_bridge.o $(BUILD)/dynamic_dynamics.o $(BUILD)/dynamic_rk4.o $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_math.o $(BUILD)/dynamic_torque.o -lm -o $(BUILD)/dyn_main_global_noncollinear_sada_flag1_two_step_gold_compare
	$(BUILD)/dyn_main_global_noncollinear_sada_flag1_two_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_noncollinear_wheel_limit_two_step_compare.c $(BUILD)/dynamic_dyn_main_globals.o $(BUILD)/dynamic_core_input_globals.o $(BUILD)/dynamic_core_input.o $(BUILD)/dynamic_mainout_globals.o $(BUILD)/dynamic_measure_globals.o $(BUILD)/dynamic_device_globals.o $(BUILD)/dynamic_telemetry.o $(BUILD)/dynamic_ipc_telemetry.o $(BUILD)/dynamic_devices.o $(BUILD)/dynamic_satellite_globals.o $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_environment.o $(BUILD)/dynamic_core_environment.o $(BUILD)/dynamic_core_bridge.o $(BUILD)/dynamic_dynamics.o $(BUILD)/dynamic_rk4.o $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_math.o $(BUILD)/dynamic_torque.o -lm -o $(BUILD)/dyn_main_global_noncollinear_wheel_limit_two_step_gold_compare
	$(BUILD)/dyn_main_global_noncollinear_wheel_limit_two_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_noncollinear_sada_flag1_ten_step_compare.c $(BUILD)/dynamic_dyn_main_globals.o $(BUILD)/dynamic_core_input_globals.o $(BUILD)/dynamic_core_input.o $(BUILD)/dynamic_mainout_globals.o $(BUILD)/dynamic_measure_globals.o $(BUILD)/dynamic_device_globals.o $(BUILD)/dynamic_telemetry.o $(BUILD)/dynamic_ipc_telemetry.o $(BUILD)/dynamic_devices.o $(BUILD)/dynamic_satellite_globals.o $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_environment.o $(BUILD)/dynamic_core_environment.o $(BUILD)/dynamic_core_bridge.o $(BUILD)/dynamic_dynamics.o $(BUILD)/dynamic_rk4.o $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_math.o $(BUILD)/dynamic_torque.o -lm -o $(BUILD)/dyn_main_global_noncollinear_sada_flag1_ten_step_gold_compare
	$(BUILD)/dyn_main_global_noncollinear_sada_flag1_ten_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_noncollinear_flex_sada_flag1_ten_step_compare.c $(BUILD)/dynamic_dyn_main_globals.o $(BUILD)/dynamic_core_input_globals.o $(BUILD)/dynamic_core_input.o $(BUILD)/dynamic_mainout_globals.o $(BUILD)/dynamic_measure_globals.o $(BUILD)/dynamic_device_globals.o $(BUILD)/dynamic_telemetry.o $(BUILD)/dynamic_ipc_telemetry.o $(BUILD)/dynamic_devices.o $(BUILD)/dynamic_satellite_globals.o $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_environment.o $(BUILD)/dynamic_core_environment.o $(BUILD)/dynamic_core_bridge.o $(BUILD)/dynamic_dynamics.o $(BUILD)/dynamic_rk4.o $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_math.o $(BUILD)/dynamic_torque.o -lm -o $(BUILD)/dyn_main_global_noncollinear_flex_sada_flag1_ten_step_gold_compare
	$(BUILD)/dyn_main_global_noncollinear_flex_sada_flag1_ten_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_noncollinear_flex_sada_flag1_twenty_step_compare.c $(BUILD)/dynamic_dyn_main_globals.o $(BUILD)/dynamic_core_input_globals.o $(BUILD)/dynamic_core_input.o $(BUILD)/dynamic_mainout_globals.o $(BUILD)/dynamic_measure_globals.o $(BUILD)/dynamic_device_globals.o $(BUILD)/dynamic_telemetry.o $(BUILD)/dynamic_ipc_telemetry.o $(BUILD)/dynamic_devices.o $(BUILD)/dynamic_satellite_globals.o $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_environment.o $(BUILD)/dynamic_core_environment.o $(BUILD)/dynamic_core_bridge.o $(BUILD)/dynamic_dynamics.o $(BUILD)/dynamic_rk4.o $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_math.o $(BUILD)/dynamic_torque.o -lm -o $(BUILD)/dyn_main_global_noncollinear_flex_sada_flag1_twenty_step_gold_compare
	$(BUILD)/dyn_main_global_noncollinear_flex_sada_flag1_twenty_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_noncollinear_flag_transition_ten_step_compare.c $(BUILD)/dynamic_dyn_main_globals.o $(BUILD)/dynamic_core_input_globals.o $(BUILD)/dynamic_core_input.o $(BUILD)/dynamic_mainout_globals.o $(BUILD)/dynamic_measure_globals.o $(BUILD)/dynamic_device_globals.o $(BUILD)/dynamic_telemetry.o $(BUILD)/dynamic_ipc_telemetry.o $(BUILD)/dynamic_devices.o $(BUILD)/dynamic_satellite_globals.o $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_environment.o $(BUILD)/dynamic_core_environment.o $(BUILD)/dynamic_core_bridge.o $(BUILD)/dynamic_dynamics.o $(BUILD)/dynamic_rk4.o $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_math.o $(BUILD)/dynamic_torque.o -lm -o $(BUILD)/dyn_main_global_noncollinear_flag_transition_ten_step_gold_compare
	$(BUILD)/dyn_main_global_noncollinear_flag_transition_ten_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_noncollinear_multi_actuator_ten_step_compare.c $(BUILD)/dynamic_dyn_main_globals.o $(BUILD)/dynamic_core_input_globals.o $(BUILD)/dynamic_core_input.o $(BUILD)/dynamic_mainout_globals.o $(BUILD)/dynamic_measure_globals.o $(BUILD)/dynamic_device_globals.o $(BUILD)/dynamic_telemetry.o $(BUILD)/dynamic_ipc_telemetry.o $(BUILD)/dynamic_devices.o $(BUILD)/dynamic_satellite_globals.o $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_environment.o $(BUILD)/dynamic_core_environment.o $(BUILD)/dynamic_core_bridge.o $(BUILD)/dynamic_dynamics.o $(BUILD)/dynamic_rk4.o $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_math.o $(BUILD)/dynamic_torque.o -lm -o $(BUILD)/dyn_main_global_noncollinear_multi_actuator_ten_step_gold_compare
	$(BUILD)/dyn_main_global_noncollinear_multi_actuator_ten_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_noncollinear_flag_transition_twenty_step_compare.c $(BUILD)/dynamic_dyn_main_globals.o $(BUILD)/dynamic_core_input_globals.o $(BUILD)/dynamic_core_input.o $(BUILD)/dynamic_mainout_globals.o $(BUILD)/dynamic_measure_globals.o $(BUILD)/dynamic_device_globals.o $(BUILD)/dynamic_telemetry.o $(BUILD)/dynamic_ipc_telemetry.o $(BUILD)/dynamic_devices.o $(BUILD)/dynamic_satellite_globals.o $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_environment.o $(BUILD)/dynamic_core_environment.o $(BUILD)/dynamic_core_bridge.o $(BUILD)/dynamic_dynamics.o $(BUILD)/dynamic_rk4.o $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_math.o $(BUILD)/dynamic_torque.o -lm -o $(BUILD)/dyn_main_global_noncollinear_flag_transition_twenty_step_gold_compare
	$(BUILD)/dyn_main_global_noncollinear_flag_transition_twenty_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_noncollinear_flag_transition_seed12345_twenty_step_compare.c $(BUILD)/dynamic_dyn_main_globals.o $(BUILD)/dynamic_core_input_globals.o $(BUILD)/dynamic_core_input.o $(BUILD)/dynamic_mainout_globals.o $(BUILD)/dynamic_measure_globals.o $(BUILD)/dynamic_device_globals.o $(BUILD)/dynamic_telemetry.o $(BUILD)/dynamic_ipc_telemetry.o $(BUILD)/dynamic_devices.o $(BUILD)/dynamic_satellite_globals.o $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_environment.o $(BUILD)/dynamic_core_environment.o $(BUILD)/dynamic_core_bridge.o $(BUILD)/dynamic_dynamics.o $(BUILD)/dynamic_rk4.o $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_math.o $(BUILD)/dynamic_torque.o -lm -o $(BUILD)/dyn_main_global_noncollinear_flag_transition_seed12345_twenty_step_gold_compare
	$(BUILD)/dyn_main_global_noncollinear_flag_transition_seed12345_twenty_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/rk4_kernel_selftest.c $(BUILD)/dynamic_rk4.o -lm -o $(BUILD)/rk4_kernel_selftest
	$(BUILD)/rk4_kernel_selftest
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/m2e_gold_compare.c $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/m2e_gold_compare
	$(BUILD)/m2e_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/quat_diff_gold_compare.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/quat_diff_gold_compare
	$(BUILD)/quat_diff_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/quat_reunit_gold_compare.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/quat_reunit_gold_compare
	$(BUILD)/quat_reunit_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/eccentric2mean_gold_compare.c $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/eccentric2mean_gold_compare
	$(BUILD)/eccentric2mean_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/eccentric2true_gold_compare.c $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/eccentric2true_gold_compare
	$(BUILD)/eccentric2true_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/calc_jd_gold_compare.c $(BUILD)/dynamic_time.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/calc_jd_gold_compare
	$(BUILD)/calc_jd_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/timeadd_gold_compare.c $(BUILD)/dynamic_time.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/timeadd_gold_compare
	$(BUILD)/timeadd_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/timeadd_boundary_gold_compare.c $(BUILD)/dynamic_time.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/timeadd_boundary_gold_compare
	$(BUILD)/timeadd_boundary_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/calc_theta_gmst_gold_compare.c $(BUILD)/dynamic_time.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/calc_theta_gmst_gold_compare
	$(BUILD)/calc_theta_gmst_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/calc_precession_gold_compare.c $(BUILD)/dynamic_time.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/calc_precession_gold_compare
	$(BUILD)/calc_precession_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/gci2ecef_gold_compare.c $(BUILD)/dynamic_time.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/gci2ecef_gold_compare
	$(BUILD)/gci2ecef_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/ecef2gci_gold_compare.c $(BUILD)/dynamic_time.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/ecef2gci_gold_compare
	$(BUILD)/ecef2gci_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/elements2posvel_gold_compare.c $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/elements2posvel_gold_compare
	$(BUILD)/elements2posvel_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/elements2posvel_nu_gold_compare.c $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/elements2posvel_nu_gold_compare
	$(BUILD)/elements2posvel_nu_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/posvel2elements_gold_compare.c $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/posvel2elements_gold_compare
	$(BUILD)/posvel2elements_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/posvel2elements_degenerate_gold_compare.c $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/posvel2elements_degenerate_gold_compare
	$(BUILD)/posvel2elements_degenerate_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/orbit_dynamic_gold_compare.c $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/orbit_dynamic_gold_compare
	$(BUILD)/orbit_dynamic_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/rk4_orbit_subblock_gold_compare.c $(BUILD)/dynamic_rk4.o $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/rk4_orbit_subblock_gold_compare
	$(BUILD)/rk4_orbit_subblock_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/rk4_fullstate_nonzero_c_k_gold_compare.c $(BUILD)/dynamic_rk4.o $(BUILD)/dynamic_dynamics.o $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/rk4_fullstate_nonzero_c_k_gold_compare
	$(BUILD)/rk4_fullstate_nonzero_c_k_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/core_dynamic_default_rk4_bridge_compare.c $(BUILD)/dynamic_core_bridge.o $(BUILD)/dynamic_rk4.o $(BUILD)/dynamic_dynamics.o $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/core_dynamic_default_rk4_bridge_compare
	$(BUILD)/core_dynamic_default_rk4_bridge_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/core_dynamic_default_two_step_bridge_compare.c $(BUILD)/dynamic_core_bridge.o $(BUILD)/dynamic_rk4.o $(BUILD)/dynamic_dynamics.o $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/core_dynamic_default_two_step_bridge_compare
	$(BUILD)/core_dynamic_default_two_step_bridge_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/gravity_gradient_torque_gold_compare.c $(BUILD)/dynamic_torque.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/gravity_gradient_torque_gold_compare
	$(BUILD)/gravity_gradient_torque_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/sun_gold_compare.c $(BUILD)/dynamic_environment.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/sun_gold_compare
	$(BUILD)/sun_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/magnetic_vector_gold_compare.c $(BUILD)/dynamic_environment.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/magnetic_vector_gold_compare
	$(BUILD)/magnetic_vector_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/inertial_magnetic_gold_compare.c $(BUILD)/dynamic_environment.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/inertial_magnetic_gold_compare
	$(BUILD)/inertial_magnetic_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/core_dynamic_environment_bridge_compare.c $(BUILD)/dynamic_core_environment.o $(BUILD)/dynamic_environment.o $(BUILD)/dynamic_core_bridge.o $(BUILD)/dynamic_rk4.o $(BUILD)/dynamic_dynamics.o $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/core_dynamic_environment_bridge_compare
	$(BUILD)/core_dynamic_environment_bridge_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/dyn_main_recovered_step2_compare.c $(BUILD)/dynamic_main_bridge.o $(BUILD)/dynamic_core_environment.o $(BUILD)/dynamic_environment.o $(BUILD)/dynamic_devices.o $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_telemetry.o $(BUILD)/dynamic_ipc_telemetry.o $(BUILD)/dynamic_core_bridge.o $(BUILD)/dynamic_rk4.o $(BUILD)/dynamic_dynamics.o $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_torque.o $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_math.o -pthread -lm -o $(BUILD)/dyn_main_recovered_step2_compare
	$(BUILD)/dyn_main_recovered_step2_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/rw1_pos_core_step2_compare.c $(BUILD)/dynamic_core_environment.o $(BUILD)/dynamic_environment.o $(BUILD)/dynamic_core_bridge.o $(BUILD)/dynamic_rk4.o $(BUILD)/dynamic_dynamics.o $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/rw1_pos_core_step2_compare
	$(BUILD)/rw1_pos_core_step2_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/dyn_main_rw1_pos_step2_compare.c $(BUILD)/dynamic_main_bridge.o $(BUILD)/dynamic_core_environment.o $(BUILD)/dynamic_environment.o $(BUILD)/dynamic_devices.o $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_telemetry.o $(BUILD)/dynamic_ipc_telemetry.o $(BUILD)/dynamic_core_bridge.o $(BUILD)/dynamic_rk4.o $(BUILD)/dynamic_dynamics.o $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_torque.o $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_math.o -pthread -lm -o $(BUILD)/dyn_main_rw1_pos_step2_compare
	$(BUILD)/dyn_main_rw1_pos_step2_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/dyn_main_mtq_x_pos_step2_compare.c $(BUILD)/dynamic_main_bridge.o $(BUILD)/dynamic_core_environment.o $(BUILD)/dynamic_environment.o $(BUILD)/dynamic_devices.o $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_telemetry.o $(BUILD)/dynamic_ipc_telemetry.o $(BUILD)/dynamic_core_bridge.o $(BUILD)/dynamic_rk4.o $(BUILD)/dynamic_dynamics.o $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_torque.o $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_math.o -pthread -lm -o $(BUILD)/dyn_main_mtq_x_pos_step2_compare
	$(BUILD)/dyn_main_mtq_x_pos_step2_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/wheel_actuator_gold_compare.c $(BUILD)/dynamic_devices.o $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/wheel_actuator_gold_compare
	$(BUILD)/wheel_actuator_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/wheel_exact_boundary_gold_compare.c $(BUILD)/dynamic_devices.o $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/wheel_exact_boundary_gold_compare
	$(BUILD)/wheel_exact_boundary_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/thruster_actuator_gold_compare.c $(BUILD)/dynamic_devices.o $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/thruster_actuator_gold_compare
	$(BUILD)/thruster_actuator_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/mtq_actuator_gold_compare.c $(BUILD)/dynamic_devices.o $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/mtq_actuator_gold_compare
	$(BUILD)/mtq_actuator_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/sada_set_get_gold_compare.c $(BUILD)/dynamic_devices.o $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/sada_set_get_gold_compare
	$(BUILD)/sada_set_get_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/sada_drive_once_gold_compare.c $(BUILD)/dynamic_devices.o $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/sada_drive_once_gold_compare
	$(BUILD)/sada_drive_once_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/update_mag_moment_gold_compare.c $(BUILD)/dynamic_devices.o $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/update_mag_moment_gold_compare
	$(BUILD)/update_mag_moment_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/update_wheel_group_gold_compare.c $(BUILD)/dynamic_devices.o $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/update_wheel_group_gold_compare
	$(BUILD)/update_wheel_group_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/update_sat_inertia_flag1_gold_compare.c $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/update_sat_inertia_flag1_gold_compare
	$(BUILD)/update_sat_inertia_flag1_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/update_sat_inertia_flag0_default_gold_compare.c $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/update_sat_inertia_flag0_default_gold_compare
	$(BUILD)/update_sat_inertia_flag0_default_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/update_device_control_controlled_gold_compare.c $(BUILD)/dynamic_devices.o $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/update_device_control_controlled_gold_compare
	$(BUILD)/update_device_control_controlled_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/gyro_magmeter_gold_compare.c $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/gyro_magmeter_gold_compare
	$(BUILD)/gyro_magmeter_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/update_gps_gold_compare.c $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/update_gps_gold_compare
	$(BUILD)/update_gps_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/dss_gold_compare.c $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/dss_gold_compare
	$(BUILD)/dss_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/dss_valid_flag_gold_compare.c $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/dss_valid_flag_gold_compare
	$(BUILD)/dss_valid_flag_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/sts_quat_gold_compare.c $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/sts_quat_gold_compare
	$(BUILD)/sts_quat_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/get_sts_quat_gold_compare.c $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/get_sts_quat_gold_compare
	$(BUILD)/get_sts_quat_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/sts_valid_flag_gold_compare.c $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/sts_valid_flag_gold_compare
	$(BUILD)/sts_valid_flag_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/update_device_measure_dispatch_compare.c $(BUILD)/dynamic_sensor_schedule.o -lm -o $(BUILD)/update_device_measure_dispatch_compare
	$(BUILD)/update_device_measure_dispatch_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/update_device_measure_recovered_gold_compare.c $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/update_device_measure_recovered_gold_compare
	$(BUILD)/update_device_measure_recovered_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/send_dyn_tele_compare.c $(BUILD)/dynamic_ipc_telemetry.o -lm -o $(BUILD)/send_dyn_tele_compare
	$(BUILD)/send_dyn_tele_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/dyn_main_send_dyn_tele_gold_compare.c $(BUILD)/dynamic_ipc_telemetry.o -pthread -lm -o $(BUILD)/dyn_main_send_dyn_tele_gold_compare
	$(BUILD)/dyn_main_send_dyn_tele_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/update_main_out_pack_compare.c $(BUILD)/dynamic_telemetry.o $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/update_main_out_pack_compare
	$(BUILD)/update_main_out_pack_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/telemetry_device_adapter_gold_compare.c $(BUILD)/dynamic_telemetry.o $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/telemetry_device_adapter_gold_compare
	$(BUILD)/telemetry_device_adapter_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/vector2angle_gold_compare.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/vector2angle_gold_compare
	$(BUILD)/vector2angle_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/is_earth_out_gold_compare.c $(BUILD)/dynamic_sensors.o $(BUILD)/dynamic_math.o -lm -o $(BUILD)/is_earth_out_gold_compare
	$(BUILD)/is_earth_out_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/time_orbit/quat_att_mat_gold_compare.c $(BUILD)/dynamic_math.o -lm -o $(BUILD)/quat_att_mat_gold_compare
	$(BUILD)/quat_att_mat_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) -DGOLD=\"analysis/golden/core_dynamic_global_abi/\" analysis/core_dynamic_global_diagnostic_compare.c $(BUILD)/dynamic_satellite_globals.o $(BUILD)/dynamic_core_bridge.o $(BUILD)/dynamic_core_environment.o $(BUILD)/dynamic_dynamics.o $(BUILD)/dynamic_rk4.o $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_environment.o $(BUILD)/dynamic_math.o $(BUILD)/dynamic_torque.o -lm -o $(BUILD)/core_dynamic_global_zero_gold_compare
	$(BUILD)/core_dynamic_global_zero_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) -DGOLD=\"analysis/golden/core_dynamic_global_abi/controlled_input/\" analysis/core_dynamic_global_diagnostic_compare.c $(BUILD)/dynamic_satellite_globals.o $(BUILD)/dynamic_core_bridge.o $(BUILD)/dynamic_core_environment.o $(BUILD)/dynamic_dynamics.o $(BUILD)/dynamic_rk4.o $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_environment.o $(BUILD)/dynamic_math.o $(BUILD)/dynamic_torque.o -lm -o $(BUILD)/core_dynamic_global_controlled_gold_compare
	$(BUILD)/core_dynamic_global_controlled_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) -DGOLD=\"analysis/golden/core_dynamic_global_abi/controlled_attitude/\" analysis/core_dynamic_global_diagnostic_compare.c $(BUILD)/dynamic_satellite_globals.o $(BUILD)/dynamic_core_bridge.o $(BUILD)/dynamic_core_environment.o $(BUILD)/dynamic_dynamics.o $(BUILD)/dynamic_rk4.o $(BUILD)/dynamic_flex.o $(BUILD)/dynamic_orbit.o $(BUILD)/dynamic_time.o $(BUILD)/dynamic_environment.o $(BUILD)/dynamic_math.o $(BUILD)/dynamic_torque.o -lm -o $(BUILD)/core_dynamic_global_attitude_gold_compare
	$(BUILD)/core_dynamic_global_attitude_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_orbit_time_boundary_ten_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_global_orbit_time_boundary_ten_step_gold_compare
	$(BUILD)/dyn_main_global_orbit_time_boundary_ten_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_leap_day_leo_ten_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_global_leap_day_leo_ten_step_gold_compare
	$(BUILD)/dyn_main_global_leap_day_leo_ten_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_dss0_noise_ten_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_global_dss0_noise_ten_step_gold_compare
	$(BUILD)/dyn_main_global_dss0_noise_ten_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_dss1_noise_ten_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_global_dss1_noise_ten_step_gold_compare
	$(BUILD)/dyn_main_global_dss1_noise_ten_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_dss_both_off_ten_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_global_dss_both_off_ten_step_gold_compare
	$(BUILD)/dyn_main_global_dss_both_off_ten_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_dss_custom_noise_ten_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_global_dss_custom_noise_ten_step_gold_compare
	$(BUILD)/dyn_main_global_dss_custom_noise_ten_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_dss_custom_noise_seed12345_ten_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_global_dss_custom_noise_seed12345_ten_step_gold_compare
	$(BUILD)/dyn_main_global_dss_custom_noise_seed12345_ten_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_y_noncollinear_two_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_global_y_noncollinear_two_step_gold_compare
	$(BUILD)/dyn_main_global_y_noncollinear_two_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_y_dss_custom_ten_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_global_y_dss_custom_ten_step_gold_compare
	$(BUILD)/dyn_main_global_y_dss_custom_ten_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_y_noncollinear_flex_ten_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_global_y_noncollinear_flex_ten_step_gold_compare
	$(BUILD)/dyn_main_global_y_noncollinear_flex_ten_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_y_multi_actuator_ten_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_global_y_multi_actuator_ten_step_gold_compare
	$(BUILD)/dyn_main_global_y_multi_actuator_ten_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_y_orbit_time_boundary_ten_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_global_y_orbit_time_boundary_ten_step_gold_compare
	$(BUILD)/dyn_main_global_y_orbit_time_boundary_ten_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_y_leap_day_leo_ten_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_global_y_leap_day_leo_ten_step_gold_compare
	$(BUILD)/dyn_main_global_y_leap_day_leo_ten_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_y_sada_flag1_ten_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_global_y_sada_flag1_ten_step_gold_compare
	$(BUILD)/dyn_main_global_y_sada_flag1_ten_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_flex_observability_ten_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_flex_observability_ten_step_gold_compare
	$(BUILD)/dyn_main_flex_observability_ten_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_flex_observability_twenty_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_flex_observability_twenty_step_gold_compare
	$(BUILD)/dyn_main_flex_observability_twenty_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_altq_flex_amplitude_ten_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_altq_flex_amplitude_ten_step_gold_compare
	$(BUILD)/dyn_main_altq_flex_amplitude_ten_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_altq_flex_amplitude_ten_step_seed12345_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_altq_flex_amplitude_seed12345_ten_step_gold_compare
	$(BUILD)/dyn_main_altq_flex_amplitude_seed12345_ten_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_altq_flex_amplitude_seed12345_twenty_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_altq_flex_amplitude_seed12345_twenty_step_gold_compare
	$(BUILD)/dyn_main_altq_flex_amplitude_seed12345_twenty_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_high_ecc_altq_fullflex_ten_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_high_ecc_altq_fullflex_ten_step_gold_compare
	$(BUILD)/dyn_main_high_ecc_altq_fullflex_ten_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_high_ecc_shadow_adjacent_date_ten_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_high_ecc_shadow_adjacent_date_ten_step_gold_compare
	$(BUILD)/dyn_main_high_ecc_shadow_adjacent_date_ten_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_high_ecc_shadow_adjacent_date_twenty_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_high_ecc_shadow_adjacent_date_twenty_step_gold_compare
	$(BUILD)/dyn_main_high_ecc_shadow_adjacent_date_twenty_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_high_ecc_shadow_adjacent_date_fifty_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_high_ecc_shadow_adjacent_date_fifty_step_gold_compare
	$(BUILD)/dyn_main_high_ecc_shadow_adjacent_date_fifty_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_high_ecc_shadow_adjacent_date_hundred_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_high_ecc_shadow_adjacent_date_hundred_step_gold_compare
	$(BUILD)/dyn_main_high_ecc_shadow_adjacent_date_hundred_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_high_ecc_shadow_adjacent_date_thousand_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_high_ecc_shadow_adjacent_date_thousand_step_gold_compare
	$(BUILD)/dyn_main_high_ecc_shadow_adjacent_date_thousand_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_mixed_command_hundred_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_global_mixed_command_hundred_step_gold_compare
	$(BUILD)/dyn_main_global_mixed_command_hundred_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_mixed_command_thousand_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_global_mixed_command_thousand_step_gold_compare
	$(BUILD)/dyn_main_global_mixed_command_thousand_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_leap_day_altq_fullflex_hundred_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_leap_day_altq_fullflex_hundred_step_gold_compare
	$(BUILD)/dyn_main_leap_day_altq_fullflex_hundred_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_leap_day_altq_fullflex_thousand_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_leap_day_altq_fullflex_thousand_step_gold_compare
	$(BUILD)/dyn_main_leap_day_altq_fullflex_thousand_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_dss_custom_noise_seed12345_hundred_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_global_dss_custom_noise_seed12345_hundred_step_gold_compare
	$(BUILD)/dyn_main_global_dss_custom_noise_seed12345_hundred_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_dss_custom_noise_seed12345_thousand_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_global_dss_custom_noise_seed12345_thousand_step_gold_compare
	$(BUILD)/dyn_main_global_dss_custom_noise_seed12345_thousand_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_noncollinear_multi_actuator_hundred_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_global_noncollinear_multi_actuator_hundred_step_gold_compare
	$(BUILD)/dyn_main_global_noncollinear_multi_actuator_hundred_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_noncollinear_multi_actuator_thousand_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_global_noncollinear_multi_actuator_thousand_step_gold_compare
	$(BUILD)/dyn_main_global_noncollinear_multi_actuator_thousand_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_sada_saturation_hundred_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_global_sada_saturation_hundred_step_gold_compare
	$(BUILD)/dyn_main_global_sada_saturation_hundred_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_sada_saturation_thousand_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_global_sada_saturation_thousand_step_gold_compare
	$(BUILD)/dyn_main_global_sada_saturation_thousand_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_leap_day_shadow_altq_fullflex_hundred_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_leap_day_shadow_altq_fullflex_hundred_step_gold_compare
	$(BUILD)/dyn_main_leap_day_shadow_altq_fullflex_hundred_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_leap_day_shadow_altq_fullflex_thousand_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_leap_day_shadow_altq_fullflex_thousand_step_gold_compare
	$(BUILD)/dyn_main_leap_day_shadow_altq_fullflex_thousand_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_noncollinear_flex_sada_flag1_hundred_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_global_noncollinear_flex_sada_flag1_hundred_step_gold_compare
	$(BUILD)/dyn_main_global_noncollinear_flex_sada_flag1_hundred_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_noncollinear_flex_sada_flag1_thousand_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_global_noncollinear_flex_sada_flag1_thousand_step_gold_compare
	$(BUILD)/dyn_main_global_noncollinear_flex_sada_flag1_thousand_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_dss_both_off_hundred_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_global_dss_both_off_hundred_step_gold_compare
	$(BUILD)/dyn_main_global_dss_both_off_hundred_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_dss_both_off_thousand_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_global_dss_both_off_thousand_step_gold_compare
	$(BUILD)/dyn_main_global_dss_both_off_thousand_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_dss0_noise_hundred_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_global_dss0_noise_hundred_step_gold_compare
	$(BUILD)/dyn_main_global_dss0_noise_hundred_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_dss0_noise_thousand_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_global_dss0_noise_thousand_step_gold_compare
	$(BUILD)/dyn_main_global_dss0_noise_thousand_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_dss1_noise_hundred_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_global_dss1_noise_hundred_step_gold_compare
	$(BUILD)/dyn_main_global_dss1_noise_hundred_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_dss1_noise_thousand_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_global_dss1_noise_thousand_step_gold_compare
	$(BUILD)/dyn_main_global_dss1_noise_thousand_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_high_ecc_thirdq_inverseflex_hundred_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_high_ecc_thirdq_inverseflex_hundred_step_gold_compare
	$(BUILD)/dyn_main_high_ecc_thirdq_inverseflex_hundred_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_high_ecc_thirdq_inverseflex_thousand_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_high_ecc_thirdq_inverseflex_thousand_step_gold_compare
	$(BUILD)/dyn_main_high_ecc_thirdq_inverseflex_thousand_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_leap_day_shadow_dss0_noise_hundred_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_leap_day_shadow_dss0_noise_hundred_step_gold_compare
	$(BUILD)/dyn_main_leap_day_shadow_dss0_noise_hundred_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_leap_day_shadow_dss0_noise_thousand_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_leap_day_shadow_dss0_noise_thousand_step_gold_compare
	$(BUILD)/dyn_main_leap_day_shadow_dss0_noise_thousand_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_altq_flex_amplitude_hundred_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_altq_flex_amplitude_hundred_step_gold_compare
	$(BUILD)/dyn_main_altq_flex_amplitude_hundred_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_altq_flex_amplitude_thousand_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_altq_flex_amplitude_thousand_step_gold_compare
	$(BUILD)/dyn_main_altq_flex_amplitude_thousand_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_high_ecc_altq_fullflex_hundred_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_high_ecc_altq_fullflex_hundred_step_gold_compare
	$(BUILD)/dyn_main_high_ecc_altq_fullflex_hundred_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_high_ecc_altq_fullflex_thousand_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_high_ecc_altq_fullflex_thousand_step_gold_compare
	$(BUILD)/dyn_main_high_ecc_altq_fullflex_thousand_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_orbit_time_boundary_hundred_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_global_orbit_time_boundary_hundred_step_gold_compare
	$(BUILD)/dyn_main_global_orbit_time_boundary_hundred_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_orbit_time_boundary_thousand_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_global_orbit_time_boundary_thousand_step_gold_compare
	$(BUILD)/dyn_main_global_orbit_time_boundary_thousand_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_leap_day_leo_hundred_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_global_leap_day_leo_hundred_step_gold_compare
	$(BUILD)/dyn_main_global_leap_day_leo_hundred_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_leap_day_leo_thousand_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_global_leap_day_leo_thousand_step_gold_compare
	$(BUILD)/dyn_main_global_leap_day_leo_thousand_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_leap_day_altq_fullflex_ten_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_leap_day_altq_fullflex_ten_step_gold_compare
	$(BUILD)/dyn_main_leap_day_altq_fullflex_ten_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_high_ecc_altq_fullflex_twenty_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_high_ecc_altq_fullflex_twenty_step_gold_compare
	$(BUILD)/dyn_main_high_ecc_altq_fullflex_twenty_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_leap_day_altq_fullflex_twenty_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_leap_day_altq_fullflex_twenty_step_gold_compare
	$(BUILD)/dyn_main_leap_day_altq_fullflex_twenty_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_high_ecc_thirdq_inverseflex_ten_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_high_ecc_thirdq_inverseflex_ten_step_gold_compare
	$(BUILD)/dyn_main_high_ecc_thirdq_inverseflex_ten_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_leap_day_shadow_altq_fullflex_ten_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_leap_day_shadow_altq_fullflex_ten_step_gold_compare
	$(BUILD)/dyn_main_leap_day_shadow_altq_fullflex_ten_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_leap_day_shadow_dss0_noise_ten_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_leap_day_shadow_dss0_noise_ten_step_gold_compare
	$(BUILD)/dyn_main_leap_day_shadow_dss0_noise_ten_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_zero_hundred_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_global_zero_hundred_step_gold_compare
	$(BUILD)/dyn_main_global_zero_hundred_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_global_zero_thousand_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_global_zero_thousand_step_gold_compare
	$(BUILD)/dyn_main_global_zero_thousand_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_multi_actuator_global_y_hundred_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_multi_actuator_global_y_hundred_step_gold_compare
	$(BUILD)/dyn_main_multi_actuator_global_y_hundred_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_multi_actuator_global_y_thousand_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_multi_actuator_global_y_thousand_step_gold_compare
	$(BUILD)/dyn_main_multi_actuator_global_y_thousand_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_seeded_command_schedule_hundred_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_seeded_command_schedule_hundred_step_gold_compare
	$(BUILD)/dyn_main_seeded_command_schedule_hundred_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_seeded_command_schedule_thousand_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_seeded_command_schedule_thousand_step_gold_compare
	$(BUILD)/dyn_main_seeded_command_schedule_thousand_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_cross_flex_dss_seeded_command_hundred_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_cross_flex_dss_seeded_command_hundred_step_gold_compare
	$(BUILD)/dyn_main_cross_flex_dss_seeded_command_hundred_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_cross_flex_dss_seeded_command_thousand_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_cross_flex_dss_seeded_command_thousand_step_gold_compare
	$(BUILD)/dyn_main_cross_flex_dss_seeded_command_thousand_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_cross_flex_dss_altseed_command_hundred_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_cross_flex_dss_altseed_command_hundred_step_gold_compare
	$(BUILD)/dyn_main_cross_flex_dss_altseed_command_hundred_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_cross_flex_dss_altseed_command_thousand_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_cross_flex_dss_altseed_command_thousand_step_gold_compare
	$(BUILD)/dyn_main_cross_flex_dss_altseed_command_thousand_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_cross_flex_dss_altseed_command_ten_thousand_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_cross_flex_dss_altseed_command_ten_thousand_step_gold_compare
	$(BUILD)/dyn_main_cross_flex_dss_altseed_command_ten_thousand_step_gold_compare
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_cross_flex_dss_altseed_command_hundred_thousand_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_cross_flex_dss_altseed_command_hundred_thousand_step_gold_compare
	$(BUILD)/dyn_main_cross_flex_dss_altseed_command_hundred_thousand_step_gold_compare

check-h0-fifty: all
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_high_ecc_shadow_adjacent_date_fifty_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_high_ecc_shadow_adjacent_date_fifty_step_gold_compare
	$(BUILD)/dyn_main_high_ecc_shadow_adjacent_date_fifty_step_gold_compare

check-h0-hundred: all
	$(CC) $(CPPFLAGS) $(CFLAGS) analysis/dyn_main_high_ecc_shadow_adjacent_date_hundred_step_compare.c $(LIB) -lm -o $(BUILD)/dyn_main_high_ecc_shadow_adjacent_date_hundred_step_gold_compare
	$(BUILD)/dyn_main_high_ecc_shadow_adjacent_date_hundred_step_gold_compare

clean:
	rm -rf $(BUILD)

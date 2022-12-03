#ifndef PKG_PACKAGE_HPP
#define PKG_PACKAGE_HPP

// pkgs

// for c++
#ifdef __cplusplus
#include <std_api/std_api.hpp>
#include <can_smbus/can_smbus.hpp>
#include <odom_driver/odom_driver.hpp>
#include <robomas/robomas.hpp>
#include <controller_client/controller_client.hpp>
#endif


#ifdef __cplusplus
extern "C" {
#endif

void PKGInitializer_init(SABool_t is_can_smbus, size_t gw_count);

#ifdef __cplusplus
}
#endif


#endif /*PKG_PACKAGE_HPP*/
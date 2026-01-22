/**
 * @brief Temperature sensor interface.
 */
#pragma once

#include <stdint.h>

#include "driver/tempsensor/interface.h"

namespace driver
{
namespace tempsensor
{
/**
 * @brief Temperature sensor stub.
 */
class Stub final : public Interface
{
public:
    /**
    * @brief Constructor
    */
    Stub() noexcept
    : myTemp{0}
    , myInitialized{true}
    {}

    /**
     * @brief Destructor.
     */
    ~Stub() noexcept override = default;

    /**
     * @brief Check if the temperature sensor is initialized.
     * 
     * @return True if the temperature sensor is initialized, false otherwise.
     */
    bool isInitialized() const noexcept override { return myInitialized; }

    /**
     * @brief Read the temperature sensor.
     *
     * @return The temperature in degrees Celsius.
     */
    int16_t read() const noexcept override { return myTemp; } 

    /**
     *  @brief Set new temperature value
     * 
     *  @return The new temperature in degrees Celsius.
     */
    void setTemperature(int16_t newTemp) const noexcept { newTemp = myTemp; }

    Stub(const Stub&)            = delete; // No copy constructor.
    Stub(Stub&&)                 = delete; // No move constructor.
    Stub& operator=(const Stub&) = delete; // No copy assignment.
    Stub& operator=(Stub&&)      = delete; // No move assignment.

private:
    /** Temperatur sensor read value */
    int16_t myTemp;

    /** Temperature sensor initialization state (true = initialized). */
    bool myInitialized;

};
} // namespace tempsensor
} // namespace driver

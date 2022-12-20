# System Requirements

The design must meet the following requirements
1. Must be able to measure down to 1g accuracy from a 500kg 
    1. Must be able to measure 0.0001% of Full Scale of a weight sensor in compression (do not need to account for tension)
    2. Must be able to poll at minimum 250Hz
    3. Can amplify the signal
2. Must be able to detect if bad connection with safety interlocks
    1. Must have a stop switch
    2. Must detect when lid is lifted and stop all motor activity
    3. Must have current sensing for motor
    4. Must have limit switches for the motor
    5. Must be able to detect the type of safety interlock
    6. Must use a standarized polarized connector
3. Must be able to react to safety interlocks within 1ms
4. Must be able to fully control the device over USB
5. Must have CLI over USB
6. Must have tested hardware (unit testing)
7. Must store configuration between reboots







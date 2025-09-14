# Simple ATM Controller for ROS 2

A simple, dependency-free ATM (Automated Teller Machine) controller library implemented in C++ with Google Test. This project is designed to be built and tested within the ROS 2 (Humble) environment.

The controller logic is decoupled from any UI, database, or hardware, focusing purely on the state flow of an ATM session.

---

## Features

-   **Standard ATM Flow:** Implements the core logic for `Insert Card` => `Verify PIN` => `Select Account` => `See Balance / Deposit / Withdraw`.
-   **Decoupled Design:** Uses an abstract interface (`IBankService`) to separate the controller logic from the bank's backend system, making it highly testable.
-   **State Management:** Internally manages the session state to prevent invalid operations (e.g., withdrawing before PIN verification).
-   **Test-Driven:** Comes with a comprehensive set of unit tests using the Google Test and Google Mock frameworks.
-   **ROS 2 Integration:** Packaged as a standard ROS 2 `ament_cmake` library, ready to be integrated into a larger ROS 2 workspace.

---

## Prerequisites

-   **ROS 2 Humble Hawksbill:** This package is built and tested on ROS 2 Humble.
-   **colcon:** The standard ROS 2 build tool.

---

## How to Build and Run Tests

Follow these steps to build the library and execute the unit tests.

### 1. Clone the Repository

Clone this repository into your ROS 2 workspace's `src` directory.

```bash
cd ~/your_ros2_ws/src
git clone https://github.com/Wonseok-Oh/simple_atm_controller.git
```

### 2. Build the Package
Navigate to the root of your workspace and build the package using `colcon`.

```bash
cd ~/your_ros2_ws
colcon build --packages-select simple_atm_controller
```
### 3. Source the Workspace
After a successful build, source the workspace's setup file to make the package available to the ROS 2 environment.
```bash
source install/setup.bash
```
### 4. Run the Tests
Execute the tests using `colcon`. The `--event-handlers console_direct+` flag is recommended to see detailed results if any tests fail.
```bash
colcon test --packages-select simple_atm_controller --event-handlers console_direct+
```
You should see an output indicating that all test cases have passed.

## Project Structure
```
├── CMakeLists.txt
├── include
│   └── simple_atm_controller
│       └── atm_controller.hpp
├── LICENSE
├── package.xml
├── README.md
└── src
    ├── atm_controller.cpp
    └── test
        └── test_atm_controller.cpp

4 directories, 7 files
```
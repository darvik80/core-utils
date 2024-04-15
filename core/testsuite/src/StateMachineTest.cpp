//
// Created by Ivan Kishchenko on 05.02.2022.
//

#include "StateMachineTest.h"
#include "StateMachine.h"

namespace network {

    BOOST_FIXTURE_TEST_SUITE(StateMachineTest, StateMachineFixture)

        BOOST_AUTO_TEST_CASE(testMainFunctions) {
            std::string state;

            struct OpenEvent {
            };

            struct CloseEvent {
            };

            struct ClosedState;
            struct OpenState;

            struct ClosedState {
                [[nodiscard]] TransitionTo<OpenState> handle(const OpenEvent &) const {
                    std::cout << "Opening the door..." << std::endl;
                    return {};
                }

                Nothing handle(const CloseEvent &) const {
                    std::cout << "Cannot close. The door is already closed!" << std::endl;
                    return {};
                }
            };

            struct OpenState {
                [[nodiscard]] TransitionTo<ClosedState> handle(const CloseEvent &) const {
                    std::cout << "Closing the door..." << std::endl;
                    return {};
                }
            };

            using Door = StateMachine<ClosedState, OpenState>;

            Door door;
            door.handle(OpenEvent{});
            BOOST_CHECK_NO_THROW(std::get<OpenState*>(door.getCurrentState()));
            BOOST_CHECK_THROW(std::get<ClosedState*>(door.getCurrentState()), std::bad_variant_access);
            door.handle(CloseEvent{});
            BOOST_CHECK_NO_THROW(std::get<ClosedState*>(door.getCurrentState()));
            BOOST_CHECK_THROW(std::get<OpenState*>(door.getCurrentState()), std::bad_variant_access);
            door.handle(CloseEvent{});
            BOOST_CHECK_NO_THROW(std::get<ClosedState*>(door.getCurrentState()));
            BOOST_CHECK_THROW(std::get<OpenState*>(door.getCurrentState()), std::bad_variant_access);
            door.handle(OpenEvent{});
            BOOST_CHECK_NO_THROW(std::get<OpenState*>(door.getCurrentState()));
            BOOST_CHECK_THROW(std::get<ClosedState*>(door.getCurrentState()), std::bad_variant_access);
        }

    BOOST_AUTO_TEST_SUITE_END()

}
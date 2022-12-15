# FSM Treadmill
Creating a finite state machine based on a treadmill
![Treadmill picture](/doc/pics/treadmill.svg)

## Authors
* C. van Dreumel
* J.J. Groenendijk

## Goals
As student embedded systems engineers, understanding the concept of finite state machines is important because they provide a conceptual framework for modeling and designing the behavior of digital systems. Finite state machines are used to design and implement the control logic of many different types of systems, including everything from simple home appliances to complex industrial control systems. By understanding the principles of finite state machines, we can develop more effective and efficient control systems for a wide range of applications. Additionally, understanding finite state machines can help us better analyze and troubleshoot problems that may arise in existing control systems, and it can provide a foundation for learning more advanced topics in control engineering.

## To Do

| Weeknummer | Beschrijving                                                                                                                                |
|------------|---------------------------------------------------------------------------------------------------------------------------------------------|
| Week 1     | Plan presenteren Aanpak en toelichting Vastleggen teams en plannen                                                                          |
| Week 2     | Functionele specificaties beschreven in product document                                                                                    |
| Week 3     | 1e versie Architectuur en State Chart			 Werken aan de inleidende opdracht (zie hoofdstuk 5 studiegids)                                        |
| Week 4     | Definitieve versie Architectuur en State Chart                                                                                              |
| Week 5     | Implementatie in de software `4. Van State Chart naar FSM in het programma v0.1.pdf` `FSM_Framework_0.2.zip` `FSM_FrameworkModular-0.2.zip` |
| Week 6     | Implementatie in de software                                                                                                                |
| Week 7     | Afronden en testen programma. Product rapport afronden                                                                                      |

### Deadline
Uiterlijk 5 Januari 2022 Programma opleveren en waarna de assesments worden ingepland

## State Chart
This UML code describes a state chart for a treadmill machine. It shows the different states the machine can be in, and the events that can trigger a transition between states. For example, when the machine is turned on, it goes into the S_INIT state. If the user then starts the machine, it transitions to the S_STANDBY state. From there, the user can either start running, which takes the machine to the S_DEFAULT state, or enter diagnostics mode, which takes it to the S_DIAGNOSTICS state.
Each state is described by a set of actions that the machine performs while in that state. For example, in the S_DEFAULT state, the machine applies a brake to the degree of tilt and applies a constant power to the speed motor. It also allows changes to the tilt and speed to be made.
Overall, this state chart describes the behavior of a treadmill machine and the different actions it can perform based on user input and other events.
[treadmill-state-chart.uml](/uml/treadmill-state-chart.uml)
![treadmill-state-chart](/uml/treadmill-state-chart.svg)


## State Architecture
Een vereenvoudigd model van de systeem architectuur ziet er als volgt uit:
[treadmill-architecture.uml](/uml/treadmill-architecture.uml)
![treadmill-architecture](/uml/treadmill-architecture.svg)

## License
MIT

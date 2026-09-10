# Forgotten
Overly ambitious UE project

This project serves as a technical demonstration for the Voxta Plugin within Unreal Engine.

Full design doc: https://docs.google.com/document/d/1v4YIVGK3a4jeyPQwxP9kBmlifpGE05u5T2pBD4JELDI  
Trello board: https://trello.com/b/xFdesJSx/forgotten  
Voxta Plugin: https://github.com/grrimgrriefer/UnrealVoxta / https://dev.azure.com/grrimgrriefer/UnrealVoxta

# Coding conventions

Mostly follows UE's condings standards with the following exceptions:

* Member variables always start with `m_`.
  * Private & protected members use `m_camelCase`.
  * Public members use `m_PascalCase`.
* Local variables & parameters always are camelCase.
* When using multiple inheritance, all overrides must be grouped in `#pragma region basename` blocks
* Header files cannot contain function definitions.
* Lines cannot extend 130 chars
  * When parameters have to be split, every parameter must be on a newline in both .h and .cpp files.
* Virtual private functions should be avoided.
* Global using-declarations should be avoided.

## Header file layout

* Includes
* Forward declarations
* Class summary
* Friend declarations
* Public
  * Statics
  * Using-declarations
  * Constructor
  * Delegates (not delegathandles)
  * Virtuals & Overrides
  * Functions
  * UFunctions
  * UProperties
* Protected
  * Virtuals & Overrides
  * Functions
  * UFunctions
  * UProperties
  * Member fields
* Private
  * Functions
  * UFunctions
  * UPROPERTIES
  * Member fields

_CPP file must implement functions in the same order as used in the header file._

# Context

The user plays as a psychologist that's talking with a woman named Rain.
Set in a somewhat grounded sci-fi setting, with some cosmic horror elements later on.

The core is mainly investigation and deduction, where you have to piece things together based on what Rain says in your conversations.
Gathering clues, fragmented memories and later one hallucinations etc. Mainly just the interaction between the user and the AI driving most things so the plugin can be fleshed out and tested in an actual use case.

# Core states

```mermaid
---
config:
  theme: dark
---
stateDiagram-v2
    state Main_StateTree {
        state Menu {
            Main_Menu --> Continue
            Main_Menu --> [*] : Quit Game
        }

        Menu --> Gameplay_State : Continue
        Gameplay_State --> Menu : Exit
    }

    state Player_StateTree {
        Player_Spawn --> Player_Action

        state Player_Action {
            state Focused_Conversation {
                Dialogue_Active --> Fail_State : Meter reaches 0/100
                Fail_State --> Dialogue_Active : Use Rewind
            }
            
            state Notebook {
                Transcript --> Deduction_Board : Annotate Clue
                Deduction_Board --> Inquiry_Angles : Link Clues (Correlation/Contradiction)
                Transcript --> System_Rewind : Use Charge
            }
            
            Freeroam --> Inspect_Object : Inspect 3D Object
            Inspect_Object --> Freeroam : Release
            Freeroam --> Focus_Mode : Focus (Hallucinations)
            Focus_Mode --> Freeroam : Release

            Freeroam --> Seated : Sit Down
            Seated --> Freeroam : Stand Up
            Freeroam --> Focused_Conversation : Focused Conversation
            Focused_Conversation --> Freeroam : End Session
            Focused_Conversation --> Notebook : Open
            Notebook --> Focused_Conversation : Close
            Freeroam --> Notebook : Open
            Notebook --> Freeroam : Close
        }

        Player_Action --> Scripted_event : Event trigger
        Scripted_event --> Player_Action : Event ended
      
        Player_Action --> Evaluation_Phase : End the day
        
        state Evaluation_Phase {
            Assemble_Report --> Submit_Report : Redact/Include Evidence
        }
        
        Evaluation_Phase --> Player_Action : Next Day
    }
    
    state NPC_StateTree {
        NPC_Spawn --> NPC_Ambient_Behavior

        state NPC_Ambient_Behavior {
            NPC_Idle --> NPC_Room_Specific_Task : Perform Chore / Eerie Habit
            NPC_Room_Specific_Task --> NPC_Idle : Finish Task
        }
        
        state NPC_Interaction {
            NPC_Proximity_Awareness --> NPC_Greet_Player : Player Enters Room
            NPC_Greet_Player --> NPC_Conversation : Player Initiates
        }
        
        NPC_Ambient_Behavior --> NPC_Interaction : Player Approaches
        NPC_Interaction --> NPC_Ambient_Behavior : Player Leaves
    }
    
    state Voxta_StateTree {
        Voxta_Disconnected --> Voxta_AttemptingToConnect : Start handshake
        Voxta_AttemptingToConnect --> Voxta_Idle : Handshake successful
        Voxta_Idle --> Voxta_Chatting : User or NPC initiated chat
        Voxta_Chatting --> Voxta_Idle : User or NPC ended chat
        Voxta_Chatting --> Voxta_Disconnected : Error or connection loss
        Voxta_Idle --> Voxta_Disconnected : Error or connection loss

        state Voxta_Chatting {
            Voxta_GeneratingReply --> Voxta_AudioPlayback : Audio generated and imported into UE
            Voxta_AudioPlayback --> Voxta_WaitingForUserResponse : Audio finished & Mic is activated
            Voxta_GeneratingReply --> Voxta_WaitingForUserResponse : Reply was text-only
            Voxta_WaitingForUserResponse --> Voxta_GeneratingReply : Speech started & completed events triggered
        }
    }
    
    state Narrative_StateTree {
        Phase_1_Arrival --> Phase_2_Exploring : Sleep / End Day 1
        Phase_2_Exploring --> Phase_3_Descent : Main Clue Nodes Linked
        Phase_3_Descent --> Phase_4_Awakening : Touch Artifact
        Phase_4_Awakening --> Endings : Final Clue Choice
    }
```

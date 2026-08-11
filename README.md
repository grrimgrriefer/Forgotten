# Forgotten
Overly ambitious UE project

This project serves as a technical demonstration for the Voxta Plugin within Unreal Engine.

Full design doc: https://docs.google.com/document/d/1v4YIVGK3a4jeyPQwxP9kBmlifpGE05u5T2pBD4JELDI  
Trello board: https://trello.com/b/xFdesJSx/forgotten  
Voxta Plugin: https://github.com/grrimgrriefer/UnrealVoxta / https://dev.azure.com/grrimgrriefer/UnrealVoxta

# Coding conventions

Mostly follows UE's condings standards with the following exceptions:

* Member variables, always start with `m_`
* Private & protected ones are camelCase, public ones are Pascal.
* Local variables & parameters always are camelCase.
* When using multiple inheritance, all overrides must be grouped in `#pragma region basename` blocks
* Header files cannot contain function definitions.
* Lines cannot extend 120 chars
  * When parameters are split, every parameter must be on a newline in both .h and .cpp files.

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
    state Main_Flow {
        state Menu {
            Main_Menu --> Settings
            Main_Menu --> Load_GameSave
            Settings --> Main_Menu
            Load_GameSave --> Main_Menu
            Main_Menu --> [*] : Exit Game
        }

        Menu --> Gameplay_State : Continue || New_Game || Load_Save

        Intro --> Menu

        state Gameplay_State {
            state Player_Action {           
                state Roaming {
                    Walking --> Focus_Mode : Interact (Hallucinations)
                    Focus_Mode --> Walking : Release
                }
                
                state Conversation {
                    Dialogue_Active --> Fail_State : Meter reaches 0/100
                    Fail_State --> Dialogue_Active : Use Rewind
                }
                
                state Notebook {
                    Transcript --> Deduction_Board : Annotate Clue
                    Deduction_Board --> Inquiry_Angles : Link Clues
                    Transcript --> System_Rewind : Use Charge
                }
                
                Roaming --> Notebook : Open
                Notebook --> Roaming : Close
                Roaming --> Conversation : Sit with Rain
                Conversation --> Roaming : End Session
                Conversation --> Notebook : Open
                Notebook --> Conversation : Close
            }

            Player_Action --> Scripted_event : Event trigger
            Scripted_event --> Player_Action : Event ended

            state Scripted_event {
                Cutscene
            }
            
            Player_Action --> Evaluation_Phase : End the day
            
            state Evaluation_Phase {
                Assemble_Report --> Submit_Report : Redact/Include Evidence
            }
            
            Evaluation_Phase --> Player_Action : Next Day
        }
    }
    
    state Voxta_Connection {
        Voxta_Idle --> Voxta_Listening : Player Inputs
        Voxta_Listening --> Voxta_Processing : Input Submitted
        Voxta_Processing --> Voxta_Synthesizing : LLM Context Generated
        Voxta_Synthesizing --> Voxta_Speaking : Audio Ready
        Voxta_Speaking --> Voxta_Idle : Finished Speaking
        
        Voxta_Processing --> Voxta_Error : Timeout
        Voxta_Error --> Voxta_Idle : Mask/Recover
    }
    
    state Narrative_Phase {
        Phase_1_Arrival --> Phase_2_Exploring : Sleep / End Day 1
        Phase_2_Exploring --> Phase_3_Descent : 5 Clue Nodes Linked
        Phase_3_Descent --> Phase_4_Awakening : Touch Artifact
        Phase_4_Awakening --> Endings : Final Clue Choice
    }
```

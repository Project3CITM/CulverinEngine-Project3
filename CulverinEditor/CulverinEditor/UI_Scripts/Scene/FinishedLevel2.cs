using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;



   public class FinishedLevel2: CulverinBehaviour
   {
        void Update()
        {
            if (Input.GetKeyDown(KeyCode.P))
            {
                SceneManager.LoadNewWalkableMap("Map_Level_3_FinalVersion");
                SceneManager.LoadScene("Level_3_v3");
            }
        }

        void OnTriggerEnter()
        {
          SceneManager.LoadNewWalkableMap("Map_Level_3_FinalVersion");
          SceneManager.LoadScene("Level_3_v2");            
        }               
    }



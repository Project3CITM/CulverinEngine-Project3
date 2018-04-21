using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;



   public class FinishedLevel2: CulverinBehaviour
   {
        void Update()
        {
            if (Input.GetKeyDown(KeyCode.P))
            {
                SceneManager.LoadNewWalkableMap("Map_Level_3_final");
                SceneManager.LoadScene("Level_3_v2");
            }
        }

        void OnTriggerEnter()
        {
          SceneManager.LoadNewWalkableMap("Map_Level_3_final");
          SceneManager.LoadScene("Level_3_v2");            
        }               
    }



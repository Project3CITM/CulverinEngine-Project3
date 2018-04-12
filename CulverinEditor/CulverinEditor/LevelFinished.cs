using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;



   public class LevelFinished : CulverinBehaviour
   {
        void Update()
        {
            if (Input.GetKeyDown(KeyCode.P))
            {
                SceneManager.LoadScene("ScoreMenu");
            }
        }

        void OnTriggerEnter()
        {
            SceneManager.LoadScene("ScoreMenu");            
        }               
    }



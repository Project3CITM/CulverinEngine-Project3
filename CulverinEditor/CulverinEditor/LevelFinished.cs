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
            Debug.Log("Restarting");
            SceneManager.LoadScene("ScoreMenu");            
        }               
    }



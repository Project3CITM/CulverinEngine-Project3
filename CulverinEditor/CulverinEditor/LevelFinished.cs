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
            GameObject obj = GetComponent<CompCollider>().GetCollidedObject();
            if(obj != null && obj.CompareTag("player"))
            {
                SceneManager.LoadScene("ScoreMenu");
            }
        }               
    }



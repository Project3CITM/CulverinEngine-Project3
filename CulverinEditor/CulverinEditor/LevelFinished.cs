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

        Debug.Log("[red] FINISHING LEVEL");
        CompCollider col = GetComponent<CompCollider>();
        GameObject obj = null;
        if (col != null)
        {
            obj = col.GetCollidedObject();
        }

          if (obj != null && obj.CompareTag("player"))
           {
             Debug.Log("Restarting");
             SceneManager.LoadScene("ScoreMenu");
          }
        }               
    }



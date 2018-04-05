using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.CompilerServices;
using CulverinEditor.SceneManagement;


namespace CulverinEditor
{
    public class LevelFinished : CulverinBehaviour
    {
        void OnTriggerEnter()
        {
            GameObject obj = GetComponent<CompCollider>().GetCollidedObject();
            if(obj != null && obj.CompareTag("player"))
            {
                SceneManager.LoadScene("ScoreMenu");
            }
        }               
    }

       
}

using CulverinEditor;
using CulverinEditor.SceneManagement;

public class Game : CulverinBehaviour
{
    void Update()
    {
        if(Input.GetInput_KeyDown("FinalSkill","Player"))
        {
            SceneManager.LoadScene("Pause");
        }
    }
}
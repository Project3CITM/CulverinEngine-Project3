using CulverinEditor;
using CulverinEditor.SceneManagement;

public class Game : CulverinBehaviour
{
    void Start()
    {
        Input.SetInputManagerActive("GUI", false);
    }

    void Update()
    {
        if(Input.GetInput_KeyDown("FinalSkill","Player"))
        {
            SceneManager.LoadScene("Pause");
        }
    }
}
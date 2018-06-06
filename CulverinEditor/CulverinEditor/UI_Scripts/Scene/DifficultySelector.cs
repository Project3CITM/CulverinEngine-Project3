using CulverinEditor;
using CulverinEditor.Debug;



public class DifficultySelector : CulverinBehaviour
{
    static public bool hard_mode = false;

    public GameObject easy_button;
    public GameObject hard_button;

    void Start()
    {
        easy_button = GetLinkedObject("easy_button");
        hard_button = GetLinkedObject("hard_button");
        easy_button.GetComponent<CompButton>().Deactivate();
    }

    public void SelectEasy()
    {
        GetComponent<CompAudio>().PlayEvent("Push");
        easy_button.GetComponent<CompButton>().Deactivate();
        hard_button.GetComponent<CompButton>().Activate();
        hard_mode = false;
    }

    public void SelectHard()
    {
        GetComponent<CompAudio>().PlayEvent("Push");
        easy_button.GetComponent<CompButton>().Activate();
        hard_button.GetComponent<CompButton>().Deactivate();
        hard_mode = true;
    }

}
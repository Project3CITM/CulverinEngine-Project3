using CulverinEditor;
using CulverinEditor.Debug;



public class DifficultySelector : CulverinBehaviour
{
    static public bool hard_mode = false;

    public void SelectEasy()
    {
        GetComponent<CompAudio>().PlayEvent("Push");
        hard_mode = false;
    }

    public void SelectHard()
    {
        GetComponent<CompAudio>().PlayEvent("Push");
        hard_mode = true;
    }

}
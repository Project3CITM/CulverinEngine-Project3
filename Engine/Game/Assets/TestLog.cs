using CulverinEditor;
using CulverinEditor.Debug;

//To check string variables and Log functionality
public class TestLog
{
    public int name;
    public string surname;

    void Start()
    {
        name = 1;
        surname = "Elliot";
    }

    void Update()
    {
        string final_name = name.ToString() + surname;
        Debug.Log(final_name);
    }
}
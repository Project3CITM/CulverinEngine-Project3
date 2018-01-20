using CulverinEditor;
using CulverinEditor.Debug;

//To check string variables and Log functionality
public class TestLog
{
    public string name;
    public string surname;

    void Start()
    {
        name = "Jordi";
        surname = "Elliot";
    }

    void Update()
    {
        string final_name = name + surname;
        Debug.Log(final_name);
    }
}
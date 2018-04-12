using CulverinEditor;
using CulverinEditor.Debug;

//To check string variables and Log functionality
public class Test
{
    public Testelliot elliot;
    public string name = "Jordi";
    public string surname = "Elliot";
    public float a = 3.0f;

    void Start()
    {
        //elliot = GameObject.gameObject.GetComponent<Testelliot>();
    }

    void Update()
    {
        if (Input.KeyDown("W"))
        {
            a = elliot.AwesomeNumber();
        }
    }

    public void PrintName()
    {
        Debug.Log(name + surname);
    }
}
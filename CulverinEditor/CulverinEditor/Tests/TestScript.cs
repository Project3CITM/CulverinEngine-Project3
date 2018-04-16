using CulverinEditor;
using CulverinEditor.SceneManagement;


class TestScript : CulverinBehaviour
{
    public GameObject magic;
    public int number;
    public bool active;
    bool test = true;
    void Start()
    {
        number = 24;
        magic = GetLinkedObject("magic");
        active = false;
        magic.SetActive(active);
    }

    void Update()
    {
        if (Input.GetKeyDown(KeyCode.Space))
        {
            number++;
        }
        if (Input.GetKeyDown(KeyCode.U))
        {
            active = !active;
            if (test == true)
            {
                test = false;
              //  magic = GetLinkedObject("magic");
            }
            magic.SetActive(active);
        }
    }
}
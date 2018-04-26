using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;


public class PuzzleRotation : CulverinBehaviour
{
    public GameObject circle1;
    public GameObject circle2;
    public GameObject circle3;
    public int selected = 0;
    public float speed = 10.0f;
    public float marge = 3.0f;
    public GameObject compledgo;
    // Use this for initialization
    void Start()
    {
        circle1 = GetLinkedObject("circle1");
        circle2 = GetLinkedObject("circle2");
        circle3 = GetLinkedObject("circle3");
        compledgo.SetActive(false);
        circle1.transform.SetRotation(new Vector3(0, 0, Random.Range(0, 360)));
        circle2.transform.SetRotation(new Vector3(0, 0, Random.Range(0, 360)));
        circle3.transform.SetRotation(new Vector3(0, 0, Random.Range(0, 360)));
    }

    // Update is called once per frame
    void Update()
    {
        if (Input.GetKeyRepeat(KeyCode.A))
        {
            Vector3 temp = GetActual().transform.GetRotation();
            temp.z += speed * Time.deltaTime;
            GetActual().transform.SetRotation(temp);
        }
        if (Input.GetKeyRepeat(KeyCode.D))
        {
            Vector3 temp = GetActual().transform.GetRotation();
            temp.z += -speed * Time.deltaTime;
            GetActual().transform.SetRotation(temp);
        }
        if (Input.GetKeyDown(KeyCode.W))
        {
            if (selected > 0)
            {
                selected--;
            }
        }
        if (Input.GetKeyDown(KeyCode.S))
        {
            if (selected < 2)
            {
                selected++;
            }
        }
        bool compled = true;
        for (int i = 0; i < 3; i++)
        {
            Debug.Log(GetActual().transform.GetGlobalRotation().z);
            if (GetActual().transform.GetGlobalRotation().z > marge)
            {
                compled = false;
            }
        }
        if (compled)
        {
            compledgo.SetActive(true);
        }
    }

    GameObject GetActual()
    {
        switch(selected)
        {
            case 0:
                {
                    return circle1;
                }
            case 1:
                {
                    return circle2;
                }
            case 2:
                {
                    return circle3;
                }
        }
        return null;
    }

}
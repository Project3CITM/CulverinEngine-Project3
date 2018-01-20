using CulverinEditor;
using CulverinEditor.Debug;

public class Tank_script
{

    public GameObject tank;
    public float speed;
    public float rotationSpeed;

    void Start()
    {
        speed = 2;
        rotationSpeed = 2;
    }

    void Update()
    {
        //test = 2;
        //Debug.Log("J");
        //Debug.Log(tank.name);
        //tank.GetComponent<Transform>().Loge("Elliot");v
        float translation = Input.GetMouseXAxis() * speed;
        float rotation = Input.GetMouseYAxis() * rotationSpeed;
        translation *= Time.DeltaTime();
        rotation *= Time.DeltaTime();

        Vector3 temp = Vector3.Zero;
        temp.Set(3, 3, 3);

        tank.GetComponent<Transform>().Rotation += temp;

        //GameObject.gameObject.GetComponent<Transform>().Position = temp;
        temp.Set(5, 5, 5);
        if (Input.KeyRepeat("Space"))
        {
            Debug.Log("Rotation");
            tank.GetComponent<Transform>().Rotation.x += 3;
        }
        if (Input.KeyRepeat("Up"))
        {
            tank.GetComponent<Transform>().Position += Vector3.Forward;
            GameObject.gameObject.GetComponent<Transform>().Position += Vector3.Forward;
        }
        if (Input.KeyRepeat("Down"))
        {
            tank.GetComponent<Transform>().Position += Vector3.Backward;
            GameObject.gameObject.GetComponent<Transform>().Position += Vector3.Backward;
        }
        if (Input.KeyRepeat("Left"))
        {
            tank.GetComponent<Transform>().Position -= Vector3.Left;
            GameObject.gameObject.GetComponent<Transform>().Position -= Vector3.Left;
        }
        if (Input.KeyRepeat("Right"))
        {
            tank.GetComponent<Transform>().Position -= Vector3.Right;
            GameObject.gameObject.GetComponent<Transform>().Position -= Vector3.Right;
        }
    }
}
using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.Map;

//Attach this script to the tank parent object if you want to see it rotate
public class TestMovement
{
    private float movSpeed = 0.0f;
    Vector3 endPosition;
    int distanceToMove = 10;


    void Start()
    {
        int[,] array2Da;//
        array2Da = new int[3, 2];
        array2Da[0,0] = 1;
        Debug.Log(array2Da[0,0].ToString());
        endPosition = GameObject.gameObject.GetComponent<Transform>().Position;
        string map = Map.GetMapString();
        Debug.Log(map);
    }

    void Update()
    {

        GameObject.gameObject.GetComponent<Transform>().Position = Vector3.MoveTowards(GameObject.gameObject.GetComponent<Transform>().Position, endPosition, movSpeed * Time.DeltaTime());

        if (Input.KeyDown("A")) //Left
        {
            endPosition = new Vector3(endPosition.x + distanceToMove, endPosition.y, endPosition.z);
        }
        else if (Input.KeyDown("D")) //Right
        {
            endPosition = new Vector3(endPosition.x - distanceToMove, endPosition.y, endPosition.z);
        }
        else if (Input.KeyDown("W")) //Up
        {
            endPosition = new Vector3(endPosition.x, endPosition.y, endPosition.z + distanceToMove);
        }
        else if (Input.KeyDown("S")) //Down
        {
            endPosition = new Vector3(endPosition.x, endPosition.y, endPosition.z - distanceToMove);
        }
    }
}
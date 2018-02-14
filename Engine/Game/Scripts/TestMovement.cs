using CulverinEditor;
using CulverinEditor.Debug;

//Attach this script to the tank parent object if you want to see it rotate
public class myTank
{
    private float movSpeed = 0.0f;
    Vector3 endPosition;
    int distanceToMove = 10;

    void Start()
    {
        endPosition = GameObject.gameObject.GetComponent<Transform>().Position;
    }

    void Update()
    {
        GameObject.gameObject.GetComponent<Transform>().Position = Vector3.MoveTowards(GameObject.gameObject.GetComponent<Transform>().Position, endPosition, movSpeed * Time.DeltaTime());

        if (Input.KeyDown("A")) //Left
        {
            endPosition = new Vector3(endPosition.x + distanceToMove, endPosition.y, endPosition.z);
        }
        if (Input.KeyDown("D")) //Right
        {
            endPosition = new Vector3(endPosition.x - distanceToMove, endPosition.y, endPosition.z);
        }
        if (Input.KeyDown("W")) //Up
        {
            endPosition = new Vector3(endPosition.x, endPosition.y, endPosition.z + distanceToMove);
        }
        if (Input.KeyDown("S")) //Down
        {
            endPosition = new Vector3(endPosition.x, endPosition.y, endPosition.z - distanceToMove);
        }
    }
}
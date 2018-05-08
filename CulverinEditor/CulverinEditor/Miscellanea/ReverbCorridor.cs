using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.Map;

public class ReverbCorridor: CulverinBehaviour
{

    public float in_value = 0.0f;
    public float out_value = 0.0f;
    public string bus_name = "Corridor";

    CompCollider col;


    void Start()
    {
        col = GetComponent<CompCollider>();
    }

    void OnTriggerEnter()
    {
        Debug.Log("REVERB ON", Department.STAGE, Color.YELLOW);
        col = GetComponent<CompCollider>();
        GameObject col_obj = col.GetCollidedObject();
        CompAudio col_audio = null;
        if (col_obj != null)
        {
            col_audio = col_obj.GetComponent<CompAudio>();
        }
        else
        {
            Debug.Log("COL_OBJ IS NULL!", Department.STAGE, Color.RED);
        }

        if (col_audio != null)
        {
            Debug.Log(in_value);
            col_audio.SetAuxiliarySends(bus_name, in_value);
        }
        else
        {
            Debug.Log("COL_AUDIO IS NULL!", Department.STAGE, Color.RED);
        }

    }


    void OnTriggerLost()
    {
        Debug.Log("REVERB OUT", Department.STAGE, Color.RED);
        col = GetComponent<CompCollider>();
        GameObject col_obj = col.GetCollidedObject();
        CompAudio col_audio = null;
        if (col_obj != null)
        {
            col_audio = col_obj.GetComponent<CompAudio>();
        }
        if (col_audio != null)
        {
            Debug.Log("REVERB OUT", Department.STAGE);
            col_audio.SetAuxiliarySends(bus_name, out_value);
        }
    }

}
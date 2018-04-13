using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.Map;

public class ReverbCorridor: CulverinBehaviour
{

    public float in_value = 0.0f;
  //  public float out_value = 0.0f;
    public string bus_name = "Corridor";

    CompCollider col;


    void Start()
    {
        col = GetComponent<CompCollider>();
    }

    void OnTriggerEnter()
    {
        col = GetComponent<CompCollider>();
        GameObject col_obj = col.GetCollidedObject();
        CompAudio col_audio = null;
        if (col_obj != null)
        {
            col_audio = col_obj.GetComponent<CompAudio>();
        }
        else
        {
            Debug.Log("[red] COL_OBJ IS NULL!");
        }

        if (col_audio != null)
        {
            Debug.Log("[green] REVERB IN");
            Debug.Log(in_value);
            col_audio.SetAuxiliarySends(bus_name, in_value);
        }
        else
        {
            Debug.Log("[red] COL_AUDIO IS NULL!");
        }

    }


    /*void OnTriggerLost()
    {
        Debug.Log("[green] REVERB OUT");
        GameObject col_obj = col.GetCollidedObject();
        CompAudio col_audio = null;
        if (col_obj != null)
        {
            col_audio = col_obj.GetComponent<CompAudio>();
        }
        if (col_audio != null)
        {
            col_audio.SetAuxiliarySends(bus_name, out_value);
        }
    }*/

}
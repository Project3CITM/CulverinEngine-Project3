using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.Map;
using CulverinEditor.SceneManagement;

public class FloorBlood : CulverinBehaviour
{
    public float max_expansion = 1.0f;
    public float expansion_factor = 1.0f;
    Vector3 scale;

    void Start()
    {
        scale = transform.GetScale();
        Debug.Log(scale, Department.PLAYER, Color.RED);
    }

    void Update()
    {
        if (scale.x < max_expansion) 
        {
            scale.x += Time.deltaTime * (expansion_factor / 10.0f);
            scale.z += Time.deltaTime * (expansion_factor / 10.0f);

            transform.SetScale(scale);
            Debug.Log(scale, Department.PLAYER, Color.RED);
        }
    }
}
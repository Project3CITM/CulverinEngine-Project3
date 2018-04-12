using CulverinEditor;
using CulverinEditor.Debug;

public class Results : CulverinBehaviour
{

    public bool render = false;
    public GameObject finish_stats;
    CompImage finish_sprite;
    
    void Start()
    {
        finish_stats = GetLinkedObject("finish_stats");
        finish_sprite.SetToRender
    }
    void Update()
    {
        finish_sprite = finish_stats.GetComponent<CompImage>();
    }


}
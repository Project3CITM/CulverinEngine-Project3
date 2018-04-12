// ------- New Script Culverin Engine 3D ------- 

using CulverinEditor;
using CulverinEditor.Debug;

public class TestAnimation : CulverinBehaviour{
    CompAnimation animation;
	// Use this for initialization
	void Start() {
        animation = GetComponent<CompAnimation>();
	}

	// Update is called once per frame
	void Update() {
        if(Input.GetKeyDown(KeyCode.T))
        {
            animation.SetTransition("ToAttack");
        }
        if (Input.GetKeyDown(KeyCode.I))
        {
            animation.PlayAnimation("Idle");
        }
    }
}

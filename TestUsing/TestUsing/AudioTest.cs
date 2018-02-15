

using CulverinEditor;
using CulverinEditor.Debug;

public class AudioTest : CulverinBehaviour
{
	// Use this for initialization
	void Start() {
       CompAudio comp_audio = GameObject.gameObject.GetComponent<CompAudio>();
       comp_audio.PlayEvent("Shot");
	}
}

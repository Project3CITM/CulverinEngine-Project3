

using CulverinEditor;
using CulverinEditor.Debug;

public class AudioTest : CulverinBehaviour
{
	// Use this for initialization
	void Start() {
        //CompAudio comp_audio = GetComponent<CompAudio>();
        //comp_audio.PlayEvent("Shot");
        Audio.ChangeVolume(3);
        Audio.Mute(true);
        Audio.ChangeState("none", "none");
    }
}

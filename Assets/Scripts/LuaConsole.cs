
using System;
using System.Text;
using UdonSharp;
using UnityEngine;
using UnityEngine.UI;
using VRC.SDKBase;
using VRC.Udon;

[UdonBehaviourSyncMode(BehaviourSyncMode.Manual)]
public class LuaConsole : UdonSharpBehaviour
{
    [SerializeField] InputField sourceField;
    [SerializeField] Text outputText;
    [SerializeField] GameObject runButton;
    [SerializeField] GameObject stopButton;
    [SerializeField] Lua lua;
    [SerializeField] class_wasi_snapshot_preview1 wasi;
    [SerializeField] string defaultSource;

    [UdonSynced, FieldChangeCallback(nameof(Source))] string _source;
    public string Source
    {
        get => _source;
        set
        {
            _source = value;
            sourceField.text = value;
        }
    }

    int sourcePtr;

    bool _running;
    bool Running
    {
        get => _running;
        set
        {
            _running = value;
            runButton.SetActive(!value);
            stopButton.SetActive(value);
        }
    }

    public long execTime = 50;

    void Start()
    {
        if (defaultSource != "" && Networking.IsMaster)
        {
            Source = defaultSource;
            SyncSource();
        }
    }

    public void SyncSource()
    {
        Networking.SetOwner(Networking.LocalPlayer, gameObject);

        Source = sourceField.text;
        RequestSerialization();
    }

    public void Run()
    {
        outputText.text = "";

        lua.wasi_snapshot_preview1 = wasi;
        wasi.wasm = lua;
        wasi.outputText = outputText;

        lua.w2us_init();
        lua._initialize();

        var sourceBytes = Encoding.UTF8.GetBytes(Source);
        sourcePtr = lua.malloc(sourceBytes.Length + 1);

        byte[] memory = lua.memory;
        sourceBytes.CopyTo(memory, sourcePtr);
        memory[sourcePtr + sourceBytes.Length] = 0;

        Running = true;
    }

    public void Stop()
    {
        Running = false;
    }

    void Update()
    {
        if (!Running) return;

        lua.set_yield_time((DateTimeOffset.UtcNow.ToUnixTimeMilliseconds() + execTime) * 1_000_000);
        int ret = lua.run(sourcePtr);
        if (ret != 1) Running = false;
    }
}

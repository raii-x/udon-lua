
using System;
using System.Text;
using UdonSharp;
using UnityEngine;
using UnityEngine.UI;
using VRC.SDKBase;
using VRC.Udon;

public class class_wasi_snapshot_preview1 : UdonSharpBehaviour
{
    [NonSerialized] public UdonSharpBehaviour wasm;
    [NonSerialized] public Text outputText;

    public int clock_time_get(int id, long precision, int time_ptr)
    {
        byte[] memory = (byte[])wasm.GetProgramVariable("memory");
        long time = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds() * 1_000_000;
        BitConverter.GetBytes(time).CopyTo(memory, time_ptr);
        return 0;
    }

    public int fd_seek(int fd, long offset, int whence, int offset_out_ptr)
    {
        return 0;
    }

    public int fd_write(int fd, int iovs_ptr, int iovs_len, int nwritten_ptr)
    {
        byte[] memory = (byte[])wasm.GetProgramVariable("memory");
        int nwritten = 0;

        for (int i = 0; i < iovs_len; i++)
        {
            int ptr = iovs_ptr + i * 8;
            int iov_ptr = BitConverter.ToInt32(memory, ptr);
            int iov_len = BitConverter.ToInt32(memory, ptr + 4);

            string str = Encoding.UTF8.GetString(memory, iov_ptr, iov_len);
            outputText.text += str;
            nwritten += iov_len;
        }

        BitConverter.GetBytes(nwritten).CopyTo(memory, nwritten_ptr);

        return 0;
    }

    public int fd_read(int fd, int iovs_ptr, int iovs_len, int nread_ptr)
    {
        return 0;
    }

    public int fd_close(int fd)
    {
        return 0;
    }

    public int fd_fdstat_get(int fd, int fdstat_ptr)
    {
        return 0;
    }

    public void proc_exit(int exit_code)
    {
        Debug.LogError($"proc_exit({exit_code})");
    }

    public int environ_sizes_get(int environ_count, int environ_size)
    {
        byte[] memory = (byte[])wasm.GetProgramVariable("memory");
        BitConverter.GetBytes(0).CopyTo(memory, environ_count);
        BitConverter.GetBytes(0).CopyTo(memory, environ_size);
        return 0;
    }

    public int environ_get(int environ, int environ_buf)
    {
        return 0;
    }
}

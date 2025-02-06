/* 
- taken directly from microsoft netcore-hosting sample: https://learn.microsoft.com/en-us/dotnet/core/tutorials/netcore-hosting
- how to make and build C# projects: https://youtu.be/KOoVm-KA8DA
- official dotnet build instructions: https://learn.microsoft.com/en-us/dotnet/core/tools/dotnet-build

- Console.WriteLine("Hello, World!");
*/

using System;
using System.Runtime.InteropServices;
using System.Threading;

using System.IO;
using System.Reflection;
using System.Runtime.Loader;
using System.Xml.Linq;
using CSScriptLib;

public interface ITestScript {
    void test_script_hello();
}

public class Program {
    private static byte isWaiting = 0;
    private static int s_CallCount = 0;
    private static ManualResetEvent mre = new ManualResetEvent(false);

    public static void Main(string[] args) {
        Console.WriteLine($"{nameof(Program)} started - args = [ {string.Join(", ", args)} ]");
        isWaiting = 1;
        mre.WaitOne();
    }

    [UnmanagedCallersOnly]
    public static byte IsWaiting() => isWaiting;

    [UnmanagedCallersOnly]
    public static void Hello(IntPtr message) {
        Console.WriteLine($"Hello, world! from {nameof(Program)} [count: {++s_CallCount}]");
        Console.WriteLine($"-- message: {Marshal.PtrToStringUTF8(message)}");
        if (s_CallCount >= 3) {
            Console.WriteLine("Signaling Program to close");
            mre.Set();
        }
    }

    // cs-script test
    [UnmanagedCallersOnly]    
    public static void LoadFile(IntPtr script_path) {
        string msg = Marshal.PtrToStringUTF8(script_path);
        dynamic test = CSScript.Evaluator.LoadFile<ITestScript>(msg);
        test.test_script_hello();
    }
}
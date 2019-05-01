using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace PerfectClearNET {
    static class Interface {
        [DllImport("sfinder.dll")]
        private static extern void action(StringBuilder str, int len);

        public static string process() {
            StringBuilder sb = new StringBuilder(500);
            action(sb, sb.Capacity);
            return sb.ToString();
        }
    }

    public static class PerfectClear {
        static PerfectClear() {}
    }
}

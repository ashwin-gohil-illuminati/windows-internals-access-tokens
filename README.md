![Language](https://img.shields.io/badge/Language-C++-blue.svg)
![Core](https://img.shields.io/badge/Core-Windows_Internals-red.svg)

---

## 📂 Access Token & Privilege Escalator

### 📝 Description
In Windows, simply being an "Administrator" does not mean your programs can automatically do whatever they want. Every process runs inside a strict Security Context defined by an "Access Token"—think of it as a digital ID badge. Whenever your program tries to open a file or access memory, a kernel subsystem called the Object Manager hands the request to the Security Reference Monitor (SRM). The SRM checks your ID badge against the resource's "Security Descriptor" (the lock). If the badge doesn't have the exact right permissions or special privileges, the kernel rejects the request, resulting in the famous "Access Denied" error. Understanding how to view and modify these digital ID badges is essential for troubleshooting permission crashes and building high-level system tools.

### 🎯 Objective
To programmatically inspect a process's own Access Token (its digital ID badge) and request the Windows Kernel to activate a dormant "God mode" power called `SeDebugPrivilege`, verifying the kernel-level change live using Process Explorer.

### 🧠 Major Underlying Concepts
* **The Key (Access Token):** A massive kernel object assigned to every process. It contains Security Identifiers (SIDs), which are the actual machine-readable numbers representing users and groups. It also holds an Integrity Level (IL) for User Account Control (UAC) and an array of system-wide powers called Privileges.
* **The Lock (Security Descriptor & DACL):** Every securable object (like a file or another process) has a lock. The main part of this lock is the Discretionary Access Control List (DACL), which is simply a top-to-bottom list of Access Control Entries (ACEs). Each ACE explicitly states whether a specific SID is allowed or denied access.
* **The Evaluator (Security Reference Monitor - SRM):** The strict kernel referee. When a program requests access, the SRM checks the Integrity Level first. Then, it reads the DACL from top to bottom—if it hits a "Deny" rule for your SID, it stops immediately and blocks you.
* **The Privilege Override (God Mode):** If the DACL denies you, the SRM performs one final check on your Access Token's privileges. If you have a specific system-wide power (like `SeDebugPrivilege`, which allows reading any process memory) and it is turned *ON*, the SRM completely bypasses the DACL and grants you access anyway.
* **Dormant Privileges & Locally Unique Identifiers (LUIDs):** Even if your token has powerful privileges, Windows keeps them marked as *Disabled* by default for safety. A program must explicitly use Win32 APIs to find the privilege's numerical ID (the LUID) and ask the OS to physically flip its attribute bit to *Enabled*.

### 🛠️ Tool Usage & Physical Justification
* **Tool Used:** Sysinternals Process Explorer (Running as Administrator)
* **What to Look For:** Run the C++ program as an Administrator (which gives it the "Full Token" instead of a stripped-down one). The program reaches into its own Access Token and asks the OS to enable the `SeDebugPrivilege`. While the program pauses, open its properties in Process Explorer and view the Security tab's Privileges list.
* **Physical Justification:** The console prints `[+] SeDebugPrivilege successfully enabled!`. In Process Explorer, the `SeDebugPrivilege` row physically shows its flag as **Enabled**. This visually proves that the Windows Kernel (specifically the Security Reference Monitor) successfully verified the Access Token, located the exact privilege, and flipped its binary attribute to 1, officially elevating the process's capabilities in real-time.

---

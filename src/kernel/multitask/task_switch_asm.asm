; Prototype C:
; void task_switch_asm(uint32_t* esp, uint32_t* ebp, uint32_t eip);

global task_switch_asm
section .text

task_switch_asm:
    ; Arguments:
    ; [esp + 4] = pointer to old esp variable
    ; [esp + 8] = pointer to old ebp variable
    ; [esp + 12] = new eip value

    ; Sauvegarder l’ESP et EBP actuels dans *esp et *ebp
    mov eax, [esp + 4]    ; eax = pointer vers old esp
    mov ebx, [esp]        ; ebx = return addr (EIP)
    mov [eax], esp        ; *esp = esp courant (pile au dessus du return addr)
    mov eax, [esp + 8]    ; eax = pointer vers old ebp
    mov [eax], ebp        ; *ebp = ebp courant

    ; Charger le nouvel ESP et EBP
    mov eax, [esp + 4]    ; eax = ptr old esp
    mov esp, [eax]        ; esp = nouvelle pile
    mov eax, [esp + 8]    ; eax = ptr old ebp
    mov ebp, [eax]        ; ebp = nouvelle base pile

    ; Récupérer le nouvel EIP
    mov eax, [esp + 12]   ; eax = nouvel eip

    ; Effectuer un saut vers eip (retour dans la tâche)
    jmp eax

#ifndef CODE_SMART_LOCK_H
#define CODE_SMART_LOCK_H

void onDeepHang();

void onNewSms(int messageId);

void onPhoneRinging(string caller_ID);

void onKeyPressed(char key);

void test_sim800_blocking();

void onExitMenuClicked();

void onSetPhoneClicked();

void unlock();

void lock();

bool isLocked();

void print_password_msg();

void print_enter_pass_to_unlock_msg();

bool isDoorOpen();

void print_status();

void onToggleLockButtonClicked();

void onDoorStateChanged(bool isOpen);

#endif //CODE_SMART_LOCK_H

with open('external_components/lt8912b/lt8912b.cpp', 'r') as f:
    content = f.read()

old_code = '''  this->io_lock_ = xSemaphoreCreateBinary();
  if (this->io_lock_ == nullptr) {
    // No memory for the lock: carry on without it rather than taking a
    // NULL semaphore (which aborts the chip) on the first flush.
    ESP_LOGW(TAG, "no memory for the DSI transfer lock - continuing without it");
  } else {
    xSemaphoreGive(this->io_lock_);
  }'''

new_code = '''  this->io_lock_ = xSemaphoreCreateBinaryStatic(&this->io_lock_buffer_);
  // Static allocation never fails; give once so the first transfer can proceed
  xSemaphoreGive(this->io_lock_);'''

content = content.replace(old_code, new_code)

with open('external_components/lt8912b/lt8912b.cpp', 'w') as f:
    f.write(content)

print('Done')
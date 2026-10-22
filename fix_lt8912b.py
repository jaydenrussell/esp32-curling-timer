with open('external_components/lt8912b/lt8912b.cpp', 'r') as f:
    content = f.read()

old_code = '''  this->io_lock_ = xSemaphoreCreateBinary();
  if (this->io_lock_ == nullptr) {
    // Without the lock the first flush would take a NULL semaphore and
    // abort the chip, so fail the component with a clear message instead.
    this->mark_failed_(LOG_STR("Could not allocate the DSI transfer lock (out of memory)"),
                       ESP_ERR_NO_MEM);
    return;
  }
  xSemaphoreGive(this->io_lock_);'''

new_code = '''  this->io_lock_ = xSemaphoreCreateBinaryStatic(&this->io_lock_buffer_);
  // Static allocation never fails; give once so the first transfer can proceed
  xSemaphoreGive(this->io_lock_);'''

content = content.replace(old_code, new_code)

with open('external_components/lt8912b/lt8912b.cpp', 'w') as f:
    f.write(content)

print('Done')
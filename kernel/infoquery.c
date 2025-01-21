#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/mm.h>

struct system_basic_information {
   unsigned long page_size;
   unsigned long num_phys_pages;
   unsigned long lowest_phys_page_number;
   unsigned long highest_phys_page_number;
   unsigned long minimum_usermode_address;
   unsigned long maximum_usermode_address;
   unsigned long active_processor_mask;
   unsigned long number_of_processors;
};

SYSCALL_DEFINE4(infoquery,
      uint8_t,                system_information_class,
      void __user *,          system_information,
      unsigned long,          system_information_length,
      unsigned long __user *, return_length) {
   if (!system_information || !system_information_length || !return_length) {
      return -EINVAL;
   }

   switch (system_information_class) {
   case 0x00: // SYSTEM_BASIC_INFORMATION
      struct system_basic_information basic_info;
      unsigned long req_len = sizeof(struct system_basic_information);
      if (system_information_length < req_len) {
         if (copy_to_user(return_length, &req_len, sizeof(unsigned long))) {
            return -EFAULT;
         }
         return -ENOMEM;
      }
      
      basic_info.page_size = PAGE_SIZE;
      basic_info.num_phys_pages = (unsigned long)totalram_pages;
      basic_info.lowest_phys_page_number = 0;
      basic_info.highest_phys_page_number = (unsigned long)totalram_pages - 1;
      basic_info.minimum_usermode_address = 0x00000000;
      basic_info.maximum_usermode_address = TASK_SIZE - 1;
      basic_info.active_processor_mask = cpumask_bits(cpu_online_mask)[0];
      basic_info.number_of_processors = num_online_cpus();

      if (copy_to_user(system_information, &basic_info, req_len)) {
         return -EFAULT;
      }

      if (copy_to_user(return_length, &req_len, sizeof(unsigned long))) {
         return -EFAULT;
      }

      break;
   default:
      return -EINVAL;
   }

   return 0;
}

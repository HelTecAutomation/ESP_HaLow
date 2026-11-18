/* Automatically generated file; DO NOT EDIT */
/* Espressif IoT Development Framework Linker Script */
/* Generated from: /home/camera/esp32/esp-idf-5.1.2/esp32-arduino-lib-builder/esp-idf/components/esp_system/ld/esp32s3/sections.ld.in */

/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Default entry point */
ENTRY(call_start_cpu0);

_diram_i_start = 0x40378000;

SECTIONS
{
  /**
   * RTC fast memory holds RTC wake stub code,
   * including from any source file named rtc_wake_stub*.c
   */
  .rtc.text :
  {
    . = ALIGN(4);
    _rtc_fast_start = ABSOLUTE(.);
    _rtc_text_start = ABSOLUTE(.);
    *(.rtc.entry.text)

    *(.rtc.literal .rtc.text .rtc.text.*)

    *rtc_wake_stub*.*(.literal .text .literal.* .text.*)
    *(.rtc_text_end_test)

    /* 16B padding for possible CPU prefetch and 4B alignment for PMS split lines */
    . += _esp_memprot_prefetch_pad_size;
    . = ALIGN(4);

    _rtc_text_end = ABSOLUTE(.);
  } > rtc_iram_seg

  /**
   * This section located in RTC FAST Memory area.
   * It holds data marked with RTC_FAST_ATTR attribute.
   * See the file "esp_attr.h" for more information.
   */
  .rtc.force_fast :
  {
    . = ALIGN(4);
    _rtc_force_fast_start = ABSOLUTE(.);

    _coredump_rtc_fast_start = ABSOLUTE(.);
    *(.rtc.fast.coredump .rtc.fast.coredump.*)
    _coredump_rtc_fast_end = ABSOLUTE(.);

    *(.rtc.force_fast .rtc.force_fast.*)
    . = ALIGN(4) ;
    _rtc_force_fast_end = ABSOLUTE(.);
  } > rtc_data_seg

  /**
   * RTC data section holds RTC wake stub
   * data/rodata, including from any source file
   * named rtc_wake_stub*.c and the data marked with
   * RTC_DATA_ATTR, RTC_RODATA_ATTR attributes.
   * The memory location of the data is dependent on
   * CONFIG_ESP32S3_RTCDATA_IN_FAST_MEM option.
   */
  .rtc.data :
  {
    _rtc_data_start = ABSOLUTE(.);

    _coredump_rtc_start = ABSOLUTE(.);
    *(.rtc.coredump .rtc.coredump.*)
    _coredump_rtc_end = ABSOLUTE(.);
    *(.rtc.data .rtc.data.*)
    *(.rtc.rodata .rtc.rodata.*)

    *rtc_wake_stub*.*(.data .rodata .data.* .rodata.*)
    _rtc_data_end = ABSOLUTE(.);
  } > rtc_data_location

  /* RTC bss, from any source file named rtc_wake_stub*.c */
  .rtc.bss (NOLOAD) :
  {
    _rtc_bss_start = ABSOLUTE(.);
    *rtc_wake_stub*.*(.bss .bss.*)
    *rtc_wake_stub*.*(COMMON)

    *(.rtc.bss)

    _rtc_bss_end = ABSOLUTE(.);
  } > rtc_data_location

  /**
   * This section holds data that should not be initialized at power up
   * and will be retained during deep sleep.
   * User data marked with RTC_NOINIT_ATTR will be placed
   * into this section. See the file "esp_attr.h" for more information.
	 * The memory location of the data is dependent on CONFIG_ESP32S3_RTCDATA_IN_FAST_MEM option.
   */
  .rtc_noinit (NOLOAD):
  {
    . = ALIGN(4);
    _rtc_noinit_start = ABSOLUTE(.);
    *(.rtc_noinit .rtc_noinit.*)
    . = ALIGN(4) ;
    _rtc_noinit_end = ABSOLUTE(.);
  } > rtc_data_location

  /**
   * This section located in RTC SLOW Memory area.
   * It holds data marked with RTC_SLOW_ATTR attribute.
   * See the file "esp_attr.h" for more information.
   */
  .rtc.force_slow :
  {
    . = ALIGN(4);
    _rtc_force_slow_start = ABSOLUTE(.);
    *(.rtc.force_slow .rtc.force_slow.*)
    . = ALIGN(4) ;
    _rtc_force_slow_end = ABSOLUTE(.);
  } > rtc_slow_seg

  /**
   * This section holds RTC data that should have fixed addresses.
   * The data are not initialized at power-up and are retained during deep sleep.
   */
  .rtc_reserved (NOLOAD):
  {
    . = ALIGN(4);
    _rtc_reserved_start = ABSOLUTE(.);
    /* New data can only be added here to ensure existing data are not moved.
       Because data have adhered to the end of the segment and code is relied on it.
       >> put new data here << */

    *(.rtc_timer_data_in_rtc_mem .rtc_timer_data_in_rtc_mem.*)
    KEEP(*(.bootloader_data_rtc_mem .bootloader_data_rtc_mem.*))
    _rtc_reserved_end = ABSOLUTE(.);
  } > rtc_reserved_seg

  _rtc_reserved_length = _rtc_reserved_end - _rtc_reserved_start;
  ASSERT((_rtc_reserved_length <= LENGTH(rtc_reserved_seg)),
          "RTC reserved segment data does not fit.")

  /* Get size of rtc slow data based on rtc_data_location alias */
  _rtc_slow_length = (ORIGIN(rtc_slow_seg) == ORIGIN(rtc_data_location))
                        ? (_rtc_force_slow_end - _rtc_data_start)
                        : (_rtc_force_slow_end - _rtc_force_slow_start);

  _rtc_fast_length = (ORIGIN(rtc_slow_seg) == ORIGIN(rtc_data_location))
                        ? (_rtc_force_fast_end - _rtc_fast_start)
                        : (_rtc_noinit_end - _rtc_fast_start);

  ASSERT((_rtc_slow_length <= LENGTH(rtc_slow_seg)),
          "RTC_SLOW segment data does not fit.")

  ASSERT((_rtc_fast_length <= LENGTH(rtc_data_seg)),
          "RTC_FAST segment data does not fit.")

  /* Send .iram0 code to iram */
  .iram0.vectors :
  {
    _iram_start = ABSOLUTE(.);
    /* Vectors go to IRAM */
    _vector_table = ABSOLUTE(.);
    . = 0x0;
    KEEP(*(.WindowVectors.text));
    . = 0x180;
    KEEP(*(.Level2InterruptVector.text));
    . = 0x1c0;
    KEEP(*(.Level3InterruptVector.text));
    . = 0x200;
    KEEP(*(.Level4InterruptVector.text));
    . = 0x240;
    KEEP(*(.Level5InterruptVector.text));
    . = 0x280;
    KEEP(*(.DebugExceptionVector.text));
    . = 0x2c0;
    KEEP(*(.NMIExceptionVector.text));
    . = 0x300;
    KEEP(*(.KernelExceptionVector.text));
    . = 0x340;
    KEEP(*(.UserExceptionVector.text));
    . = 0x3C0;
    KEEP(*(.DoubleExceptionVector.text));
    . = 0x400;
    _invalid_pc_placeholder = ABSOLUTE(.);
    *(.*Vector.literal)

    *(.UserEnter.literal);
    *(.UserEnter.text);
    . = ALIGN (16);
    *(.entry.text)
    *(.init.literal)
    *(.init)
    _init_end = ABSOLUTE(.);
  } > iram0_0_seg

  .iram0.text :
  {
    /* Code marked as running out of IRAM */
    _iram_text_start = ABSOLUTE(.);

    *(.iram1 .iram1.*)
    *libapp_trace.a:app_trace.*(.literal .literal.* .text .text.*)
    *libapp_trace.a:app_trace_util.*(.literal .literal.* .text .text.*)
    *libapp_trace.a:port_uart.*(.literal .literal.* .text .text.*)
    *libdriver.a:gptimer.*(.literal.gptimer_default_isr .text.gptimer_default_isr)
    *libesp_event.a:default_event_loop.*(.literal.esp_event_isr_post .text.esp_event_isr_post)
    *libesp_event.a:esp_event.*(.literal.esp_event_isr_post_to .text.esp_event_isr_post_to)
    *libesp_hw_support.a:cpu.*(.literal.esp_cpu_compare_and_set .text.esp_cpu_compare_and_set)
    *libesp_hw_support.a:cpu.*(.literal.esp_cpu_reset .text.esp_cpu_reset)
    *libesp_hw_support.a:cpu.*(.literal.esp_cpu_stall .text.esp_cpu_stall)
    *libesp_hw_support.a:cpu.*(.literal.esp_cpu_unstall .text.esp_cpu_unstall)
    *libesp_hw_support.a:cpu.*(.literal.esp_cpu_wait_for_intr .text.esp_cpu_wait_for_intr)
    *libesp_hw_support.a:esp_gpio_reserve.*(.literal.esp_gpio_is_pin_reserved .text.esp_gpio_is_pin_reserved)
    *libesp_hw_support.a:esp_gpio_reserve.*(.literal.esp_gpio_reserve_pins .text.esp_gpio_reserve_pins)
    *libesp_hw_support.a:esp_memory_utils.*(.literal .literal.* .text .text.*)
    *libesp_hw_support.a:mspi_timing_config.*(.literal .literal.* .text .text.*)
    *libesp_hw_support.a:mspi_timing_tuning.*(.literal .literal.* .text .text.*)
    *libesp_hw_support.a:periph_ctrl.*(.literal.periph_module_reset .text.periph_module_reset)
    *libesp_hw_support.a:periph_ctrl.*(.literal.wifi_module_disable .text.wifi_module_disable)
    *libesp_hw_support.a:periph_ctrl.*(.literal.wifi_module_enable .text.wifi_module_enable)
    *libesp_hw_support.a:rtc_clk.*(.literal .literal.* .text .text.*)
    *libesp_hw_support.a:rtc_sleep.*(.literal .literal.* .text .text.*)
    *libesp_hw_support.a:rtc_time.*(.literal .literal.* .text .text.*)
    *libesp_hw_support.a:sar_periph_ctrl.*(.literal.sar_periph_ctrl_power_enable .text.sar_periph_ctrl_power_enable)
    *libesp_hw_support.a:sleep_console.*(.literal .literal.* .text .text.*)
    *libesp_hw_support.a:systimer.*(.literal .literal.* .text .text.*)
    *libesp_mm.a:esp_cache.*(.literal .literal.* .text .text.*)
    *libesp_netif.a:esp_netif_lwip.*(.literal.esp_netif_free_rx_buffer .text.esp_netif_free_rx_buffer)
    *libesp_netif.a:esp_netif_lwip.*(.literal.esp_netif_receive .text.esp_netif_receive)
    *libesp_netif.a:esp_netif_lwip.*(.literal.esp_netif_transmit_wrap .text.esp_netif_transmit_wrap)
    *libesp_netif.a:esp_pbuf_ref.*(.literal.esp_pbuf_allocate .text.esp_pbuf_allocate)
    *libesp_netif.a:esp_pbuf_ref.*(.literal.esp_pbuf_free .text.esp_pbuf_free)
    *libesp_netif.a:ethernetif.*(.literal.ethernet_low_level_output .text.ethernet_low_level_output)
    *libesp_netif.a:ethernetif.*(.literal.ethernetif_input .text.ethernetif_input)
    *libesp_netif.a:wlanif.*(.literal.low_level_output .text.low_level_output)
    *libesp_netif.a:wlanif.*(.literal.wlanif_input .text.wlanif_input)
    *libesp_psram.a:esp_psram_impl_octal.*(.literal .literal.* .text .text.*)
    *libesp_psram.a:mmu_psram_flash.*(.literal .literal.* .text .text.*)
    *libesp_ringbuf.a:(.literal .literal.* .text .text.*)
    *libesp_rom.a:esp_rom_cache_esp32s2_esp32s3.*(.literal .literal.* .text .text.*)
    *libesp_rom.a:esp_rom_cache_writeback_esp32s3.*(.literal .literal.* .text .text.*)
    *libesp_rom.a:esp_rom_spiflash.*(.literal .literal.* .text .text.*)
    *libesp_rom.a:esp_rom_systimer.*(.literal .literal.* .text .text.*)
    *libesp_rom.a:esp_rom_wdt.*(.literal .literal.* .text .text.*)
    *libesp_system.a:esp_err.*(.literal .literal.* .text .text.*)
    *libesp_system.a:esp_system_chip.*(.literal.esp_system_abort .text.esp_system_abort)
    *libesp_system.a:ubsan.*(.literal .literal.* .text .text.*)
    *libfreertos.a:(EXCLUDE_FILE(*libfreertos.a:app_startup.* *libfreertos.a:idf_additions.*) .literal EXCLUDE_FILE(*libfreertos.a:app_startup.* *libfreertos.a:idf_additions.*) .literal.* EXCLUDE_FILE(*libfreertos.a:app_startup.* *libfreertos.a:idf_additions.*) .text EXCLUDE_FILE(*libfreertos.a:app_startup.* *libfreertos.a:idf_additions.*) .text.*)
    *libgcc.a:_divsf3.*(.literal .literal.* .text .text.*)
    *libgcc.a:lib2funcs.*(.literal .literal.* .text .text.*)
    *libgcov.a:(.literal .literal.* .text .text.*)
    *libhal.a:cache_hal.*(.literal .literal.* .text .text.*)
    *libhal.a:i2c_hal_iram.*(.literal .literal.* .text .text.*)
    *libhal.a:ledc_hal_iram.*(.literal .literal.* .text .text.*)
    *libhal.a:mmu_hal.*(.literal .literal.* .text .text.*)
    *libhal.a:spi_flash_encrypt_hal_iram.*(.literal .literal.* .text .text.*)
    *libhal.a:spi_flash_hal_gpspi.*(.literal .literal.* .text .text.*)
    *libhal.a:spi_flash_hal_iram.*(.literal .literal.* .text .text.*)
    *libhal.a:spi_hal_iram.*(.literal .literal.* .text .text.*)
    *libhal.a:spi_slave_hal_iram.*(.literal .literal.* .text .text.*)
    *libhal.a:systimer_hal.*(.literal .literal.* .text .text.*)
    *libhal.a:timer_hal.*(.literal.timer_hal_capture_and_get_counter_value .text.timer_hal_capture_and_get_counter_value)
    *libheap.a:multi_heap.*(.literal.assert_valid_block .text.assert_valid_block)
    *libheap.a:multi_heap.*(.literal.multi_heap_aligned_alloc_impl .text.multi_heap_aligned_alloc_impl)
    *libheap.a:multi_heap.*(.literal.multi_heap_aligned_alloc_impl_offs .text.multi_heap_aligned_alloc_impl_offs)
    *libheap.a:multi_heap.*(.literal.multi_heap_free_impl .text.multi_heap_free_impl)
    *libheap.a:multi_heap.*(.literal.multi_heap_get_allocated_size_impl .text.multi_heap_get_allocated_size_impl)
    *libheap.a:multi_heap.*(.literal.multi_heap_get_block_address_impl .text.multi_heap_get_block_address_impl)
    *libheap.a:multi_heap.*(.literal.multi_heap_get_first_block .text.multi_heap_get_first_block)
    *libheap.a:multi_heap.*(.literal.multi_heap_get_next_block .text.multi_heap_get_next_block)
    *libheap.a:multi_heap.*(.literal.multi_heap_internal_lock .text.multi_heap_internal_lock)
    *libheap.a:multi_heap.*(.literal.multi_heap_internal_unlock .text.multi_heap_internal_unlock)
    *libheap.a:multi_heap.*(.literal.multi_heap_is_free .text.multi_heap_is_free)
    *libheap.a:multi_heap.*(.literal.multi_heap_malloc_impl .text.multi_heap_malloc_impl)
    *libheap.a:multi_heap.*(.literal.multi_heap_realloc_impl .text.multi_heap_realloc_impl)
    *libheap.a:multi_heap.*(.literal.multi_heap_set_lock .text.multi_heap_set_lock)
    *libheap.a:multi_heap_poisoning.*(.literal.multi_heap_aligned_alloc .text.multi_heap_aligned_alloc)
    *libheap.a:multi_heap_poisoning.*(.literal.multi_heap_aligned_free .text.multi_heap_aligned_free)
    *libheap.a:multi_heap_poisoning.*(.literal.multi_heap_free .text.multi_heap_free)
    *libheap.a:multi_heap_poisoning.*(.literal.multi_heap_get_allocated_size .text.multi_heap_get_allocated_size)
    *libheap.a:multi_heap_poisoning.*(.literal.multi_heap_get_block_address .text.multi_heap_get_block_address)
    *libheap.a:multi_heap_poisoning.*(.literal.multi_heap_get_block_owner .text.multi_heap_get_block_owner)
    *libheap.a:multi_heap_poisoning.*(.literal.multi_heap_internal_check_block_poisoning .text.multi_heap_internal_check_block_poisoning)
    *libheap.a:multi_heap_poisoning.*(.literal.multi_heap_internal_poison_fill_region .text.multi_heap_internal_poison_fill_region)
    *libheap.a:multi_heap_poisoning.*(.literal.multi_heap_malloc .text.multi_heap_malloc)
    *libheap.a:multi_heap_poisoning.*(.literal.multi_heap_realloc .text.multi_heap_realloc)
    *libheap.a:multi_heap_poisoning.*(.literal.poison_allocated_region .text.poison_allocated_region)
    *libheap.a:multi_heap_poisoning.*(.literal.verify_allocated_region .text.verify_allocated_region)
    *libheap.a:tlsf.*(.literal.tlsf_align_size .text.tlsf_align_size)
    *libheap.a:tlsf.*(.literal.tlsf_alloc_overhead .text.tlsf_alloc_overhead)
    *libheap.a:tlsf.*(.literal.tlsf_block_size .text.tlsf_block_size)
    *libheap.a:tlsf.*(.literal.tlsf_block_size_max .text.tlsf_block_size_max)
    *libheap.a:tlsf.*(.literal.tlsf_block_size_min .text.tlsf_block_size_min)
    *libheap.a:tlsf.*(.literal.tlsf_free .text.tlsf_free)
    *libheap.a:tlsf.*(.literal.tlsf_get_pool .text.tlsf_get_pool)
    *libheap.a:tlsf.*(.literal.tlsf_malloc .text.tlsf_malloc)
    *libheap.a:tlsf.*(.literal.tlsf_memalign .text.tlsf_memalign)
    *libheap.a:tlsf.*(.literal.tlsf_memalign_offs .text.tlsf_memalign_offs)
    *libheap.a:tlsf.*(.literal.tlsf_realloc .text.tlsf_realloc)
    *libheap.a:tlsf.*(.literal.tlsf_size .text.tlsf_size)
    *liblog.a:log.*(.literal.esp_log_write .text.esp_log_write)
    *liblog.a:log_freertos.*(.literal.esp_log_early_timestamp .text.esp_log_early_timestamp)
    *liblog.a:log_freertos.*(.literal.esp_log_impl_lock .text.esp_log_impl_lock)
    *liblog.a:log_freertos.*(.literal.esp_log_impl_lock_timeout .text.esp_log_impl_lock_timeout)
    *liblog.a:log_freertos.*(.literal.esp_log_impl_unlock .text.esp_log_impl_unlock)
    *liblog.a:log_freertos.*(.literal.esp_log_timestamp .text.esp_log_timestamp)
    *liblwip.a:api_lib.*(.literal.netconn_recv .text.netconn_recv)
    *liblwip.a:api_lib.*(.literal.netconn_recv_data .text.netconn_recv_data)
    *liblwip.a:api_lib.*(.literal.netconn_recv_data_tcp .text.netconn_recv_data_tcp)
    *liblwip.a:api_lib.*(.literal.netconn_recv_tcp_pbuf_flags .text.netconn_recv_tcp_pbuf_flags)
    *liblwip.a:api_lib.*(.literal.netconn_recv_udp_raw_netbuf_flags .text.netconn_recv_udp_raw_netbuf_flags)
    *liblwip.a:api_lib.*(.literal.netconn_send .text.netconn_send)
    *liblwip.a:api_lib.*(.literal.netconn_sendto .text.netconn_sendto)
    *liblwip.a:api_lib.*(.literal.netconn_tcp_recvd .text.netconn_tcp_recvd)
    *liblwip.a:api_lib.*(.literal.netconn_write_partly .text.netconn_write_partly)
    *liblwip.a:api_lib.*(.literal.netconn_write_vectors_partly .text.netconn_write_vectors_partly)
    *liblwip.a:api_msg.*(.literal.lwip_netconn_do_send .text.lwip_netconn_do_send)
    *liblwip.a:api_msg.*(.literal.lwip_netconn_do_write .text.lwip_netconn_do_write)
    *liblwip.a:api_msg.*(.literal.recv_udp .text.recv_udp)
    *liblwip.a:etharp.*(.literal.etharp_output .text.etharp_output)
    *liblwip.a:etharp.*(.literal.etharp_output_to_arp_index .text.etharp_output_to_arp_index)
    *liblwip.a:ethernet.*(.literal.ethernet_input .text.ethernet_input)
    *liblwip.a:ethernet.*(.literal.ethernet_output .text.ethernet_output)
    *liblwip.a:inet_chksum.*(.literal.inet_chksum .text.inet_chksum)
    *liblwip.a:inet_chksum.*(.literal.inet_chksum_pbuf .text.inet_chksum_pbuf)
    *liblwip.a:inet_chksum.*(.literal.inet_chksum_pseudo .text.inet_chksum_pseudo)
    *liblwip.a:inet_chksum.*(.literal.inet_cksum_pseudo_base .text.inet_cksum_pseudo_base)
    *liblwip.a:inet_chksum.*(.literal.ip_chksum_pseudo .text.ip_chksum_pseudo)
    *liblwip.a:inet_chksum.*(.literal.lwip_standard_chksum .text.lwip_standard_chksum)
    *liblwip.a:ip4.*(.literal.ip4_input .text.ip4_input)
    *liblwip.a:ip4.*(.literal.ip4_output .text.ip4_output)
    *liblwip.a:ip4.*(.literal.ip4_output_if .text.ip4_output_if)
    *liblwip.a:ip4.*(.literal.ip4_output_if_opt .text.ip4_output_if_opt)
    *liblwip.a:ip4.*(.literal.ip4_output_if_opt_src .text.ip4_output_if_opt_src)
    *liblwip.a:ip4.*(.literal.ip4_output_if_src .text.ip4_output_if_src)
    *liblwip.a:ip4.*(.literal.ip4_route .text.ip4_route)
    *liblwip.a:ip4.*(.literal.ip4_route_src .text.ip4_route_src)
    *liblwip.a:ip4_addr.*(.literal.ip4_addr_isbroadcast_u32 .text.ip4_addr_isbroadcast_u32)
    *liblwip.a:lwip_default_hooks.*(.literal.ip4_route_src_hook .text.ip4_route_src_hook)
    *liblwip.a:mem.*(.literal.mem_malloc .text.mem_malloc)
    *liblwip.a:memp.*(.literal.memp_malloc .text.memp_malloc)
    *liblwip.a:netbuf.*(.literal.netbuf_alloc .text.netbuf_alloc)
    *liblwip.a:netbuf.*(.literal.netbuf_free .text.netbuf_free)
    *liblwip.a:pbuf.*(.literal.pbuf_add_header .text.pbuf_add_header)
    *liblwip.a:pbuf.*(.literal.pbuf_alloc .text.pbuf_alloc)
    *liblwip.a:pbuf.*(.literal.pbuf_alloced_custom .text.pbuf_alloced_custom)
    *liblwip.a:pbuf.*(.literal.pbuf_clone .text.pbuf_clone)
    *liblwip.a:pbuf.*(.literal.pbuf_copy .text.pbuf_copy)
    *liblwip.a:pbuf.*(.literal.pbuf_copy_partial_pbuf .text.pbuf_copy_partial_pbuf)
    *liblwip.a:pbuf.*(.literal.pbuf_free .text.pbuf_free)
    *liblwip.a:pbuf.*(.literal.pbuf_header .text.pbuf_header)
    *liblwip.a:pbuf.*(.literal.pbuf_remove_header .text.pbuf_remove_header)
    *liblwip.a:raw.*(.literal.raw_input .text.raw_input)
    *liblwip.a:sockets.*(.literal.done_socket .text.done_socket)
    *liblwip.a:sockets.*(.literal.event_callback .text.event_callback)
    *liblwip.a:sockets.*(.literal.lwip_recv_tcp .text.lwip_recv_tcp)
    *liblwip.a:sockets.*(.literal.lwip_recvfrom .text.lwip_recvfrom)
    *liblwip.a:sockets.*(.literal.lwip_select .text.lwip_select)
    *liblwip.a:sockets.*(.literal.lwip_send .text.lwip_send)
    *liblwip.a:sockets.*(.literal.lwip_sendto .text.lwip_sendto)
    *liblwip.a:sockets.*(.literal.tryget_socket_unconn_locked .text.tryget_socket_unconn_locked)
    *liblwip.a:sys_arch.*(.literal.sys_arch_mbox_fetch .text.sys_arch_mbox_fetch)
    *liblwip.a:sys_arch.*(.literal.sys_arch_protect .text.sys_arch_protect)
    *liblwip.a:sys_arch.*(.literal.sys_arch_sem_wait .text.sys_arch_sem_wait)
    *liblwip.a:sys_arch.*(.literal.sys_arch_unprotect .text.sys_arch_unprotect)
    *liblwip.a:sys_arch.*(.literal.sys_mbox_post .text.sys_mbox_post)
    *liblwip.a:sys_arch.*(.literal.sys_mbox_trypost .text.sys_mbox_trypost)
    *liblwip.a:sys_arch.*(.literal.sys_mutex_lock .text.sys_mutex_lock)
    *liblwip.a:sys_arch.*(.literal.sys_mutex_unlock .text.sys_mutex_unlock)
    *liblwip.a:sys_arch.*(.literal.sys_sem_signal .text.sys_sem_signal)
    *liblwip.a:sys_arch.*(.literal.sys_thread_tcpip .text.sys_thread_tcpip)
    *liblwip.a:tcp.*(.literal.tcp_fasttmr .text.tcp_fasttmr)
    *liblwip.a:tcp.*(.literal.tcp_process_refused_data .text.tcp_process_refused_data)
    *liblwip.a:tcp.*(.literal.tcp_recv_null .text.tcp_recv_null)
    *liblwip.a:tcp.*(.literal.tcp_recved .text.tcp_recved)
    *liblwip.a:tcp.*(.literal.tcp_seg_copy .text.tcp_seg_copy)
    *liblwip.a:tcp.*(.literal.tcp_seg_free .text.tcp_seg_free)
    *liblwip.a:tcp.*(.literal.tcp_segs_free .text.tcp_segs_free)
    *liblwip.a:tcp.*(.literal.tcp_slowtmr .text.tcp_slowtmr)
    *liblwip.a:tcp.*(.literal.tcp_tmr .text.tcp_tmr)
    *liblwip.a:tcp.*(.literal.tcp_update_rcv_ann_wnd .text.tcp_update_rcv_ann_wnd)
    *liblwip.a:tcp_in.*(.literal.tcp_input .text.tcp_input)
    *liblwip.a:tcp_in.*(.literal.tcp_input_delayed_close .text.tcp_input_delayed_close)
    *liblwip.a:tcp_in.*(.literal.tcp_oos_insert_segment .text.tcp_oos_insert_segment)
    *liblwip.a:tcp_in.*(.literal.tcp_parseopt .text.tcp_parseopt)
    *liblwip.a:tcp_in.*(.literal.tcp_receive .text.tcp_receive)
    *liblwip.a:tcp_out.*(.literal.tcp_create_segment .text.tcp_create_segment)
    *liblwip.a:tcp_out.*(.literal.tcp_output_control_segment .text.tcp_output_control_segment)
    *liblwip.a:tcp_out.*(.literal.tcp_rexmit .text.tcp_rexmit)
    *liblwip.a:tcp_out.*(.literal.tcp_rexmit_fast .text.tcp_rexmit_fast)
    *liblwip.a:tcp_out.*(.literal.tcp_rexmit_rto_prepare .text.tcp_rexmit_rto_prepare)
    *liblwip.a:tcp_out.*(.literal.tcp_route .text.tcp_route)
    *liblwip.a:tcp_out.*(.literal.tcp_rst .text.tcp_rst)
    *liblwip.a:tcp_out.*(.literal.tcp_send_empty_ack .text.tcp_send_empty_ack)
    *liblwip.a:tcp_out.*(.literal.tcp_split_unsent_seg .text.tcp_split_unsent_seg)
    *liblwip.a:tcp_out.*(.literal.tcp_write .text.tcp_write)
    *liblwip.a:tcpip.*(.literal.tcpip_callback .text.tcpip_callback)
    *liblwip.a:tcpip.*(.literal.tcpip_inpkt .text.tcpip_inpkt)
    *liblwip.a:tcpip.*(.literal.tcpip_input .text.tcpip_input)
    *liblwip.a:tcpip.*(.literal.tcpip_send_msg_wait_sem .text.tcpip_send_msg_wait_sem)
    *liblwip.a:tcpip.*(.literal.tcpip_thread .text.tcpip_thread)
    *liblwip.a:tcpip.*(.literal.tcpip_try_callback .text.tcpip_try_callback)
    *liblwip.a:udp.*(.literal.udp_input .text.udp_input)
    *liblwip.a:udp.*(.literal.udp_input_local_match .text.udp_input_local_match)
    *liblwip.a:udp.*(.literal.udp_send .text.udp_send)
    *liblwip.a:udp.*(.literal.udp_sendto .text.udp_sendto)
    *liblwip.a:udp.*(.literal.udp_sendto_if .text.udp_sendto_if)
    *liblwip.a:udp.*(.literal.udp_sendto_if_src .text.udp_sendto_if_src)
    *libnewlib.a:abort.*(.literal .literal.* .text .text.*)
    *libnewlib.a:assert.*(.literal .literal.* .text .text.*)
    *libnewlib.a:heap.*(.literal .literal.* .text .text.*)
    *libnewlib.a:stdatomic.*(.literal .literal.* .text .text.*)
    *librtc.a:(.literal .literal.* .text .text.*)
    *libsoc.a:lldesc.*(.literal .literal.* .text .text.*)
    *libspi_flash.a:flash_brownout_hook.*(.literal .literal.* .text .text.*)
    *libspi_flash.a:memspi_host_driver.*(.literal .literal.* .text .text.*)
    *libspi_flash.a:spi_flash_chip_boya.*(.literal .literal.* .text .text.*)
    *libspi_flash.a:spi_flash_chip_gd.*(.literal .literal.* .text .text.*)
    *libspi_flash.a:spi_flash_chip_generic.*(.literal .literal.* .text .text.*)
    *libspi_flash.a:spi_flash_chip_issi.*(.literal .literal.* .text .text.*)
    *libspi_flash.a:spi_flash_chip_mxic.*(.literal .literal.* .text .text.*)
    *libspi_flash.a:spi_flash_chip_mxic_opi.*(.literal .literal.* .text .text.*)
    *libspi_flash.a:spi_flash_chip_th.*(.literal .literal.* .text .text.*)
    *libspi_flash.a:spi_flash_chip_winbond.*(.literal .literal.* .text .text.*)
    *libspi_flash.a:spi_flash_hpm_enable.*(.literal .literal.* .text .text.*)
    *libspi_flash.a:spi_flash_oct_flash_init.*(.literal .literal.* .text .text.*)
    *libspi_flash.a:spi_flash_wrap.*(.literal .literal.* .text .text.*)
    *libxt_hal.a:(.literal .literal.* .text .text.*)
    *libxtensa.a:eri.*(.literal .literal.* .text .text.*)
    *libxtensa.a:xtensa_intr_asm.*(.literal .literal.* .text .text.*)

  } > iram0_0_seg

  /**
   * This section is required to skip .iram0.text area because iram0_0_seg and
   * dram0_0_seg reflect the same address space on different buses.
   */
  .dram0.dummy (NOLOAD):
  {
    . = ORIGIN(dram0_0_seg) + MAX(_iram_end - _diram_i_start, 0);
  } > dram0_0_seg

  .dram0.data :
  {
    _data_start = ABSOLUTE(.);
    *(.gnu.linkonce.d.*)
    *(.data1)
    *(.sdata)
    *(.sdata.*)
    *(.gnu.linkonce.s.*)
    *(.gnu.linkonce.s2.*)
    *(.jcr)

    *(EXCLUDE_FILE(*libbt.a *libbtdm_app.a) .data EXCLUDE_FILE(*libbt.a *libbtdm_app.a) .data.*)
    *(.dram1 .dram1.*)
    _coredump_dram_start = ABSOLUTE(.);
    *(.dram2.coredump .dram2.coredump.*)
    _coredump_dram_end = ABSOLUTE(.);
    *libapp_trace.a:app_trace.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libapp_trace.a:app_trace_util.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libapp_trace.a:port_uart.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    . = ALIGN(4);
    _bt_data_start = ABSOLUTE(.);
    *libbt.a:(.data .data.*)
    . = ALIGN(4);
    _bt_data_end = ABSOLUTE(.);
    . = ALIGN(4);
    _bt_controller_data_start = ABSOLUTE(.);
    *libbtdm_app.a:(.data .data.*)
    . = ALIGN(4);
    _bt_controller_data_end = ABSOLUTE(.);
    *libesp_hw_support.a:esp_memory_utils.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libesp_hw_support.a:mspi_timing_config.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libesp_hw_support.a:mspi_timing_tuning.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libesp_hw_support.a:rtc_clk.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libesp_hw_support.a:sleep_console.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libesp_hw_support.a:systimer.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libesp_mm.a:esp_cache.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libesp_psram.a:esp_psram_impl_octal.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libesp_psram.a:mmu_psram_flash.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libesp_rom.a:esp_rom_cache_esp32s2_esp32s3.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libesp_rom.a:esp_rom_cache_writeback_esp32s3.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libesp_rom.a:esp_rom_spiflash.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libesp_rom.a:esp_rom_systimer.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libesp_rom.a:esp_rom_wdt.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libesp_system.a:esp_err.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libesp_system.a:ubsan.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libfreertos.a:FreeRTOS-openocd.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libgcc.a:_divsf3.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libgcov.a:(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libhal.a:cache_hal.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libhal.a:i2c_hal_iram.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libhal.a:ledc_hal_iram.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libhal.a:mmu_hal.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libhal.a:spi_flash_encrypt_hal_iram.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libhal.a:spi_flash_hal_gpspi.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libhal.a:spi_flash_hal_iram.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libhal.a:spi_hal_iram.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libhal.a:spi_slave_hal_iram.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libhal.a:systimer_hal.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libnewlib.a:abort.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libnewlib.a:assert.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libnewlib.a:heap.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libnewlib.a:stdatomic.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libphy.a:(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libsoc.a:lldesc.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libspi_flash.a:flash_brownout_hook.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libspi_flash.a:memspi_host_driver.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libspi_flash.a:spi_flash_chip_boya.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libspi_flash.a:spi_flash_chip_gd.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libspi_flash.a:spi_flash_chip_generic.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libspi_flash.a:spi_flash_chip_issi.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libspi_flash.a:spi_flash_chip_mxic.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libspi_flash.a:spi_flash_chip_mxic_opi.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libspi_flash.a:spi_flash_chip_th.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libspi_flash.a:spi_flash_chip_winbond.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libspi_flash.a:spi_flash_hpm_enable.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libspi_flash.a:spi_flash_oct_flash_init.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libspi_flash.a:spi_flash_wrap.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)

    _data_end = ABSOLUTE(.);
    . = ALIGN(4);
  } > dram0_0_seg

  /**
   * This section holds data that should not be initialized at power up.
   * The section located in Internal SRAM memory region. The macro _NOINIT
   * can be used as attribute to place data into this section.
   * See the "esp_attr.h" file for more information.
   */
  .noinit (NOLOAD):
  {
    . = ALIGN(4);
    _noinit_start = ABSOLUTE(.);
    *(.noinit .noinit.*)
    . = ALIGN(4) ;
    _noinit_end = ABSOLUTE(.);
  } > dram0_0_seg

  /* Shared RAM */
  .dram0.bss (NOLOAD) :
  {
    . = ALIGN (8);
    _bss_start = ABSOLUTE(.);

    *(EXCLUDE_FILE(*libbt.a *libbtdm_app.a) .bss EXCLUDE_FILE(*libbt.a *libbtdm_app.a) .bss.*)
    *(.dynbss .dynsbss .gnu.linkonce.b .gnu.linkonce.b.* .gnu.linkonce.sb .gnu.linkonce.sb.* .gnu.linkonce.sb2 .gnu.linkonce.sb2.* .sbss .sbss.* .sbss2 .sbss2.* .scommon .share.mem)
    *(.ext_ram.bss .ext_ram.bss.*)
    *(EXCLUDE_FILE(*libbt.a *libbtdm_app.a) COMMON)
    . = ALIGN(4);
    _bt_bss_start = ABSOLUTE(.);
    *libbt.a:(.bss .bss.*)
    . = ALIGN(4);
    _bt_bss_end = ABSOLUTE(.);
    . = ALIGN(4);
    _bt_common_start = ABSOLUTE(.);
    *libbt.a:(COMMON)
    . = ALIGN(4);
    _bt_common_end = ABSOLUTE(.);
    . = ALIGN(4);
    _bt_controller_bss_start = ABSOLUTE(.);
    *libbtdm_app.a:(.bss .bss.*)
    . = ALIGN(4);
    _bt_controller_bss_end = ABSOLUTE(.);
    . = ALIGN(4);
    _bt_controller_common_start = ABSOLUTE(.);
    *libbtdm_app.a:(COMMON)
    . = ALIGN(4);
    _bt_controller_common_end = ABSOLUTE(.);

    *(.dynsbss)
    *(.sbss)
    *(.sbss.*)
    *(.gnu.linkonce.sb.*)
    *(.scommon)
    *(.sbss2)
    *(.sbss2.*)
    *(.gnu.linkonce.sb2.*)
    *(.dynbss)
    *(.share.mem)
    *(.gnu.linkonce.b.*)

    . = ALIGN (8);
    _bss_end = ABSOLUTE(.);
  } > dram0_0_seg

  ASSERT(((_bss_end - ORIGIN(dram0_0_seg)) <= LENGTH(dram0_0_seg)), "DRAM segment data does not fit.")

  .flash.text :
  {
    _stext = .;
    _instruction_reserved_start = ABSOLUTE(.);  /* This is a symbol marking the flash.text start, this can be used for mmu driver to maintain virtual address */
    _text_start = ABSOLUTE(.);

    *(EXCLUDE_FILE(*libesp_ringbuf.a *libfreertos.a *libgcov.a *librtc.a *libxt_hal.a *libapp_trace.a:app_trace.* *libapp_trace.a:app_trace_util.* *libapp_trace.a:port_uart.* *libdriver.a:gptimer.* *libesp_event.a:default_event_loop.* *libesp_event.a:esp_event.* *libesp_hw_support.a:cpu.* *libesp_hw_support.a:esp_gpio_reserve.* *libesp_hw_support.a:esp_memory_utils.* *libesp_hw_support.a:mspi_timing_config.* *libesp_hw_support.a:mspi_timing_tuning.* *libesp_hw_support.a:periph_ctrl.* *libesp_hw_support.a:rtc_clk.* *libesp_hw_support.a:rtc_sleep.* *libesp_hw_support.a:rtc_time.* *libesp_hw_support.a:sar_periph_ctrl.* *libesp_hw_support.a:sleep_console.* *libesp_hw_support.a:systimer.* *libesp_mm.a:esp_cache.* *libesp_netif.a:esp_netif_lwip.* *libesp_netif.a:esp_pbuf_ref.* *libesp_netif.a:ethernetif.* *libesp_netif.a:wlanif.* *libesp_psram.a:esp_psram_impl_octal.* *libesp_psram.a:mmu_psram_flash.* *libesp_rom.a:esp_rom_cache_esp32s2_esp32s3.* *libesp_rom.a:esp_rom_cache_writeback_esp32s3.* *libesp_rom.a:esp_rom_spiflash.* *libesp_rom.a:esp_rom_systimer.* *libesp_rom.a:esp_rom_wdt.* *libesp_system.a:esp_err.* *libesp_system.a:esp_system_chip.* *libesp_system.a:ubsan.* *libgcc.a:_divsf3.* *libgcc.a:lib2funcs.* *libhal.a:cache_hal.* *libhal.a:i2c_hal_iram.* *libhal.a:ledc_hal_iram.* *libhal.a:mmu_hal.* *libhal.a:spi_flash_encrypt_hal_iram.* *libhal.a:spi_flash_hal_gpspi.* *libhal.a:spi_flash_hal_iram.* *libhal.a:spi_hal_iram.* *libhal.a:spi_slave_hal_iram.* *libhal.a:systimer_hal.* *libhal.a:timer_hal.* *libheap.a:multi_heap.* *libheap.a:multi_heap_poisoning.* *libheap.a:tlsf.* *liblog.a:log.* *liblog.a:log_freertos.* *liblwip.a:api_lib.* *liblwip.a:api_msg.* *liblwip.a:etharp.* *liblwip.a:ethernet.* *liblwip.a:inet_chksum.* *liblwip.a:ip4.* *liblwip.a:ip4_addr.* *liblwip.a:lwip_default_hooks.* *liblwip.a:mem.* *liblwip.a:memp.* *liblwip.a:netbuf.* *liblwip.a:pbuf.* *liblwip.a:raw.* *liblwip.a:sockets.* *liblwip.a:sys_arch.* *liblwip.a:tcp.* *liblwip.a:tcp_in.* *liblwip.a:tcp_out.* *liblwip.a:tcpip.* *liblwip.a:udp.* *libnewlib.a:abort.* *libnewlib.a:assert.* *libnewlib.a:heap.* *libnewlib.a:stdatomic.* *libsoc.a:lldesc.* *libspi_flash.a:flash_brownout_hook.* *libspi_flash.a:memspi_host_driver.* *libspi_flash.a:spi_flash_chip_boya.* *libspi_flash.a:spi_flash_chip_gd.* *libspi_flash.a:spi_flash_chip_generic.* *libspi_flash.a:spi_flash_chip_issi.* *libspi_flash.a:spi_flash_chip_mxic.* *libspi_flash.a:spi_flash_chip_mxic_opi.* *libspi_flash.a:spi_flash_chip_th.* *libspi_flash.a:spi_flash_chip_winbond.* *libspi_flash.a:spi_flash_hpm_enable.* *libspi_flash.a:spi_flash_oct_flash_init.* *libspi_flash.a:spi_flash_wrap.* *libxtensa.a:eri.* *libxtensa.a:xtensa_intr_asm.*) .literal EXCLUDE_FILE(*libesp_ringbuf.a *libfreertos.a *libgcov.a *librtc.a *libxt_hal.a *libapp_trace.a:app_trace.* *libapp_trace.a:app_trace_util.* *libapp_trace.a:port_uart.* *libdriver.a:gptimer.* *libesp_event.a:default_event_loop.* *libesp_event.a:esp_event.* *libesp_hw_support.a:cpu.* *libesp_hw_support.a:esp_gpio_reserve.* *libesp_hw_support.a:esp_memory_utils.* *libesp_hw_support.a:mspi_timing_config.* *libesp_hw_support.a:mspi_timing_tuning.* *libesp_hw_support.a:periph_ctrl.* *libesp_hw_support.a:rtc_clk.* *libesp_hw_support.a:rtc_sleep.* *libesp_hw_support.a:rtc_time.* *libesp_hw_support.a:sar_periph_ctrl.* *libesp_hw_support.a:sleep_console.* *libesp_hw_support.a:systimer.* *libesp_mm.a:esp_cache.* *libesp_netif.a:esp_netif_lwip.* *libesp_netif.a:esp_pbuf_ref.* *libesp_netif.a:ethernetif.* *libesp_netif.a:wlanif.* *libesp_psram.a:esp_psram_impl_octal.* *libesp_psram.a:mmu_psram_flash.* *libesp_rom.a:esp_rom_cache_esp32s2_esp32s3.* *libesp_rom.a:esp_rom_cache_writeback_esp32s3.* *libesp_rom.a:esp_rom_spiflash.* *libesp_rom.a:esp_rom_systimer.* *libesp_rom.a:esp_rom_wdt.* *libesp_system.a:esp_err.* *libesp_system.a:esp_system_chip.* *libesp_system.a:ubsan.* *libgcc.a:_divsf3.* *libgcc.a:lib2funcs.* *libhal.a:cache_hal.* *libhal.a:i2c_hal_iram.* *libhal.a:ledc_hal_iram.* *libhal.a:mmu_hal.* *libhal.a:spi_flash_encrypt_hal_iram.* *libhal.a:spi_flash_hal_gpspi.* *libhal.a:spi_flash_hal_iram.* *libhal.a:spi_hal_iram.* *libhal.a:spi_slave_hal_iram.* *libhal.a:systimer_hal.* *libhal.a:timer_hal.* *libheap.a:multi_heap.* *libheap.a:multi_heap_poisoning.* *libheap.a:tlsf.* *liblog.a:log.* *liblog.a:log_freertos.* *liblwip.a:api_lib.* *liblwip.a:api_msg.* *liblwip.a:etharp.* *liblwip.a:ethernet.* *liblwip.a:inet_chksum.* *liblwip.a:ip4.* *liblwip.a:ip4_addr.* *liblwip.a:lwip_default_hooks.* *liblwip.a:mem.* *liblwip.a:memp.* *liblwip.a:netbuf.* *liblwip.a:pbuf.* *liblwip.a:raw.* *liblwip.a:sockets.* *liblwip.a:sys_arch.* *liblwip.a:tcp.* *liblwip.a:tcp_in.* *liblwip.a:tcp_out.* *liblwip.a:tcpip.* *liblwip.a:udp.* *libnewlib.a:abort.* *libnewlib.a:assert.* *libnewlib.a:heap.* *libnewlib.a:stdatomic.* *libsoc.a:lldesc.* *libspi_flash.a:flash_brownout_hook.* *libspi_flash.a:memspi_host_driver.* *libspi_flash.a:spi_flash_chip_boya.* *libspi_flash.a:spi_flash_chip_gd.* *libspi_flash.a:spi_flash_chip_generic.* *libspi_flash.a:spi_flash_chip_issi.* *libspi_flash.a:spi_flash_chip_mxic.* *libspi_flash.a:spi_flash_chip_mxic_opi.* *libspi_flash.a:spi_flash_chip_th.* *libspi_flash.a:spi_flash_chip_winbond.* *libspi_flash.a:spi_flash_hpm_enable.* *libspi_flash.a:spi_flash_oct_flash_init.* *libspi_flash.a:spi_flash_wrap.* *libxtensa.a:eri.* *libxtensa.a:xtensa_intr_asm.*) .literal.* EXCLUDE_FILE(*libesp_ringbuf.a *libfreertos.a *libgcov.a *librtc.a *libxt_hal.a *libapp_trace.a:app_trace.* *libapp_trace.a:app_trace_util.* *libapp_trace.a:port_uart.* *libdriver.a:gptimer.* *libesp_event.a:default_event_loop.* *libesp_event.a:esp_event.* *libesp_hw_support.a:cpu.* *libesp_hw_support.a:esp_gpio_reserve.* *libesp_hw_support.a:esp_memory_utils.* *libesp_hw_support.a:mspi_timing_config.* *libesp_hw_support.a:mspi_timing_tuning.* *libesp_hw_support.a:periph_ctrl.* *libesp_hw_support.a:rtc_clk.* *libesp_hw_support.a:rtc_sleep.* *libesp_hw_support.a:rtc_time.* *libesp_hw_support.a:sar_periph_ctrl.* *libesp_hw_support.a:sleep_console.* *libesp_hw_support.a:systimer.* *libesp_mm.a:esp_cache.* *libesp_netif.a:esp_netif_lwip.* *libesp_netif.a:esp_pbuf_ref.* *libesp_netif.a:ethernetif.* *libesp_netif.a:wlanif.* *libesp_psram.a:esp_psram_impl_octal.* *libesp_psram.a:mmu_psram_flash.* *libesp_rom.a:esp_rom_cache_esp32s2_esp32s3.* *libesp_rom.a:esp_rom_cache_writeback_esp32s3.* *libesp_rom.a:esp_rom_spiflash.* *libesp_rom.a:esp_rom_systimer.* *libesp_rom.a:esp_rom_wdt.* *libesp_system.a:esp_err.* *libesp_system.a:esp_system_chip.* *libesp_system.a:ubsan.* *libgcc.a:_divsf3.* *libgcc.a:lib2funcs.* *libhal.a:cache_hal.* *libhal.a:i2c_hal_iram.* *libhal.a:ledc_hal_iram.* *libhal.a:mmu_hal.* *libhal.a:spi_flash_encrypt_hal_iram.* *libhal.a:spi_flash_hal_gpspi.* *libhal.a:spi_flash_hal_iram.* *libhal.a:spi_hal_iram.* *libhal.a:spi_slave_hal_iram.* *libhal.a:systimer_hal.* *libhal.a:timer_hal.* *libheap.a:multi_heap.* *libheap.a:multi_heap_poisoning.* *libheap.a:tlsf.* *liblog.a:log.* *liblog.a:log_freertos.* *liblwip.a:api_lib.* *liblwip.a:api_msg.* *liblwip.a:etharp.* *liblwip.a:ethernet.* *liblwip.a:inet_chksum.* *liblwip.a:ip4.* *liblwip.a:ip4_addr.* *liblwip.a:lwip_default_hooks.* *liblwip.a:mem.* *liblwip.a:memp.* *liblwip.a:netbuf.* *liblwip.a:pbuf.* *liblwip.a:raw.* *liblwip.a:sockets.* *liblwip.a:sys_arch.* *liblwip.a:tcp.* *liblwip.a:tcp_in.* *liblwip.a:tcp_out.* *liblwip.a:tcpip.* *liblwip.a:udp.* *libnewlib.a:abort.* *libnewlib.a:assert.* *libnewlib.a:heap.* *libnewlib.a:stdatomic.* *libsoc.a:lldesc.* *libspi_flash.a:flash_brownout_hook.* *libspi_flash.a:memspi_host_driver.* *libspi_flash.a:spi_flash_chip_boya.* *libspi_flash.a:spi_flash_chip_gd.* *libspi_flash.a:spi_flash_chip_generic.* *libspi_flash.a:spi_flash_chip_issi.* *libspi_flash.a:spi_flash_chip_mxic.* *libspi_flash.a:spi_flash_chip_mxic_opi.* *libspi_flash.a:spi_flash_chip_th.* *libspi_flash.a:spi_flash_chip_winbond.* *libspi_flash.a:spi_flash_hpm_enable.* *libspi_flash.a:spi_flash_oct_flash_init.* *libspi_flash.a:spi_flash_wrap.* *libxtensa.a:eri.* *libxtensa.a:xtensa_intr_asm.*) .text EXCLUDE_FILE(*libesp_ringbuf.a *libfreertos.a *libgcov.a *librtc.a *libxt_hal.a *libapp_trace.a:app_trace.* *libapp_trace.a:app_trace_util.* *libapp_trace.a:port_uart.* *libdriver.a:gptimer.* *libesp_event.a:default_event_loop.* *libesp_event.a:esp_event.* *libesp_hw_support.a:cpu.* *libesp_hw_support.a:esp_gpio_reserve.* *libesp_hw_support.a:esp_memory_utils.* *libesp_hw_support.a:mspi_timing_config.* *libesp_hw_support.a:mspi_timing_tuning.* *libesp_hw_support.a:periph_ctrl.* *libesp_hw_support.a:rtc_clk.* *libesp_hw_support.a:rtc_sleep.* *libesp_hw_support.a:rtc_time.* *libesp_hw_support.a:sar_periph_ctrl.* *libesp_hw_support.a:sleep_console.* *libesp_hw_support.a:systimer.* *libesp_mm.a:esp_cache.* *libesp_netif.a:esp_netif_lwip.* *libesp_netif.a:esp_pbuf_ref.* *libesp_netif.a:ethernetif.* *libesp_netif.a:wlanif.* *libesp_psram.a:esp_psram_impl_octal.* *libesp_psram.a:mmu_psram_flash.* *libesp_rom.a:esp_rom_cache_esp32s2_esp32s3.* *libesp_rom.a:esp_rom_cache_writeback_esp32s3.* *libesp_rom.a:esp_rom_spiflash.* *libesp_rom.a:esp_rom_systimer.* *libesp_rom.a:esp_rom_wdt.* *libesp_system.a:esp_err.* *libesp_system.a:esp_system_chip.* *libesp_system.a:ubsan.* *libgcc.a:_divsf3.* *libgcc.a:lib2funcs.* *libhal.a:cache_hal.* *libhal.a:i2c_hal_iram.* *libhal.a:ledc_hal_iram.* *libhal.a:mmu_hal.* *libhal.a:spi_flash_encrypt_hal_iram.* *libhal.a:spi_flash_hal_gpspi.* *libhal.a:spi_flash_hal_iram.* *libhal.a:spi_hal_iram.* *libhal.a:spi_slave_hal_iram.* *libhal.a:systimer_hal.* *libhal.a:timer_hal.* *libheap.a:multi_heap.* *libheap.a:multi_heap_poisoning.* *libheap.a:tlsf.* *liblog.a:log.* *liblog.a:log_freertos.* *liblwip.a:api_lib.* *liblwip.a:api_msg.* *liblwip.a:etharp.* *liblwip.a:ethernet.* *liblwip.a:inet_chksum.* *liblwip.a:ip4.* *liblwip.a:ip4_addr.* *liblwip.a:lwip_default_hooks.* *liblwip.a:mem.* *liblwip.a:memp.* *liblwip.a:netbuf.* *liblwip.a:pbuf.* *liblwip.a:raw.* *liblwip.a:sockets.* *liblwip.a:sys_arch.* *liblwip.a:tcp.* *liblwip.a:tcp_in.* *liblwip.a:tcp_out.* *liblwip.a:tcpip.* *liblwip.a:udp.* *libnewlib.a:abort.* *libnewlib.a:assert.* *libnewlib.a:heap.* *libnewlib.a:stdatomic.* *libsoc.a:lldesc.* *libspi_flash.a:flash_brownout_hook.* *libspi_flash.a:memspi_host_driver.* *libspi_flash.a:spi_flash_chip_boya.* *libspi_flash.a:spi_flash_chip_gd.* *libspi_flash.a:spi_flash_chip_generic.* *libspi_flash.a:spi_flash_chip_issi.* *libspi_flash.a:spi_flash_chip_mxic.* *libspi_flash.a:spi_flash_chip_mxic_opi.* *libspi_flash.a:spi_flash_chip_th.* *libspi_flash.a:spi_flash_chip_winbond.* *libspi_flash.a:spi_flash_hpm_enable.* *libspi_flash.a:spi_flash_oct_flash_init.* *libspi_flash.a:spi_flash_wrap.* *libxtensa.a:eri.* *libxtensa.a:xtensa_intr_asm.*) .text.*)
    *(.wifi0iram .wifi0iram.*)
    *(.wifiextrairam .wifiextrairam.*)
    *(.wifiorslpiram .wifiorslpiram.*)
    *(.wifirxiram .wifirxiram.*)
    *(.wifislpiram .wifislpiram.*)
    *(.wifislprxiram .wifislprxiram.*)
    *libcoexist.a:(.coexiram .coexiram.*)
    *libcoexist.a:(.coexsleepiram .coexsleepiram.*)
    *libdriver.a:gptimer.*(.literal.gptimer_del_timer .literal.gptimer_destroy .literal.gptimer_disable .literal.gptimer_enable .literal.gptimer_get_captured_count .literal.gptimer_get_raw_count .literal.gptimer_get_resolution .literal.gptimer_new_timer .literal.gptimer_register_event_callbacks .literal.gptimer_release_group_handle .literal.gptimer_set_alarm_action .literal.gptimer_set_raw_count .literal.gptimer_start .literal.gptimer_stop .text .text.gptimer_del_timer .text.gptimer_destroy .text.gptimer_disable .text.gptimer_enable .text.gptimer_get_captured_count .text.gptimer_get_raw_count .text.gptimer_get_resolution .text.gptimer_new_timer .text.gptimer_register_event_callbacks .text.gptimer_release_group_handle .text.gptimer_set_alarm_action .text.gptimer_set_raw_count .text.gptimer_start .text.gptimer_stop)
    *libesp_event.a:default_event_loop.*(.literal.esp_event_handler_instance_register .literal.esp_event_handler_instance_unregister .literal.esp_event_handler_register .literal.esp_event_handler_unregister .literal.esp_event_loop_create_default .literal.esp_event_loop_delete_default .literal.esp_event_post .text .text.esp_event_handler_instance_register .text.esp_event_handler_instance_unregister .text.esp_event_handler_register .text.esp_event_handler_unregister .text.esp_event_loop_create_default .text.esp_event_loop_delete_default .text.esp_event_post)
    *libesp_event.a:esp_event.*(.literal.base_node_add_handler .literal.esp_event_handler_instance_register_with .literal.esp_event_handler_instance_unregister_with .literal.esp_event_handler_register_with .literal.esp_event_handler_register_with_internal .literal.esp_event_handler_unregister_with .literal.esp_event_handler_unregister_with_internal .literal.esp_event_loop_create .literal.esp_event_loop_delete .literal.esp_event_loop_run .literal.esp_event_loop_run_task .literal.esp_event_post_to .literal.handler_instances_add .literal.handler_instances_remove .literal.handler_instances_remove_all .literal.loop_node_add_handler .text .text.base_node_add_handler .text.esp_event_dump .text.esp_event_handler_instance_register_with .text.esp_event_handler_instance_unregister_with .text.esp_event_handler_register_with .text.esp_event_handler_register_with_internal .text.esp_event_handler_unregister_with .text.esp_event_handler_unregister_with_internal .text.esp_event_loop_create .text.esp_event_loop_delete .text.esp_event_loop_run .text.esp_event_loop_run_task .text.esp_event_post_to .text.handler_instances_add .text.handler_instances_remove .text.handler_instances_remove_all .text.loop_node_add_handler)
    *libesp_hw_support.a:cpu.*(.literal.esp_cpu_set_watchpoint .text .text.esp_cpu_clear_breakpoint .text.esp_cpu_clear_watchpoint .text.esp_cpu_set_breakpoint .text.esp_cpu_set_watchpoint)
    *libesp_hw_support.a:esp_gpio_reserve.*(.text)
    *libesp_hw_support.a:periph_ctrl.*(.literal.periph_ll_get_clk_en_mask .literal.periph_ll_get_rst_en_mask .literal.periph_module_disable .literal.periph_module_enable .text .text.periph_ll_get_clk_en_mask .text.periph_ll_get_rst_en_mask .text.periph_module_disable .text.periph_module_enable)
    *libesp_hw_support.a:sar_periph_ctrl.*(.literal.s_sar_power_acquire .literal.s_sar_power_release .literal.sar_periph_ctrl_adc_continuous_power_acquire .literal.sar_periph_ctrl_adc_continuous_power_release .literal.sar_periph_ctrl_adc_oneshot_power_acquire .literal.sar_periph_ctrl_adc_oneshot_power_release .literal.sar_periph_ctrl_init .literal.sar_periph_ctrl_power_disable .literal.sar_periph_ctrl_pwdet_power_acquire .literal.sar_periph_ctrl_pwdet_power_release .text .text.s_sar_power_acquire .text.s_sar_power_release .text.sar_periph_ctrl_adc_continuous_power_acquire .text.sar_periph_ctrl_adc_continuous_power_release .text.sar_periph_ctrl_adc_oneshot_power_acquire .text.sar_periph_ctrl_adc_oneshot_power_release .text.sar_periph_ctrl_init .text.sar_periph_ctrl_power_disable .text.sar_periph_ctrl_pwdet_power_acquire .text.sar_periph_ctrl_pwdet_power_release)
    *libesp_netif.a:esp_netif_lwip.*(.literal.alloc_netif_client_id .literal.esp_ip4addr_aton .literal.esp_ip4addr_ntoa .literal.esp_netif_add_ip6_address .literal.esp_netif_add_ip6_address_api .literal.esp_netif_api_cb .literal.esp_netif_attach .literal.esp_netif_bridge_add_port .literal.esp_netif_bridge_fdb_add .literal.esp_netif_bridge_fdb_remove .literal.esp_netif_create_ip6_linklocal .literal.esp_netif_create_ip6_linklocal_api .literal.esp_netif_deinit .literal.esp_netif_destroy .literal.esp_netif_destroy_api .literal.esp_netif_dhcpc_option .literal.esp_netif_dhcpc_option_api .literal.esp_netif_dhcpc_start .literal.esp_netif_dhcpc_start_api .literal.esp_netif_dhcpc_stop .literal.esp_netif_dhcpc_stop_api .literal.esp_netif_dhcps_cb .literal.esp_netif_dhcps_get_clients_by_mac .literal.esp_netif_dhcps_get_clients_by_mac_api .literal.esp_netif_dhcps_option .literal.esp_netif_dhcps_option_api .literal.esp_netif_dhcps_start .literal.esp_netif_dhcps_start_api .literal.esp_netif_dhcps_stop .literal.esp_netif_dhcps_stop_api .literal.esp_netif_down .literal.esp_netif_down_api .literal.esp_netif_find_if .literal.esp_netif_find_if_api .literal.esp_netif_get_all_ip6 .literal.esp_netif_get_all_preferred_ip6 .literal.esp_netif_get_default_netif .literal.esp_netif_get_dns_info .literal.esp_netif_get_dns_info_api .literal.esp_netif_get_handle_from_ifkey .literal.esp_netif_get_handle_from_netif_impl .literal.esp_netif_get_hostname .literal.esp_netif_get_ip6_global .literal.esp_netif_get_ip6_linklocal .literal.esp_netif_get_ip_info .literal.esp_netif_get_mac .literal.esp_netif_get_netif_impl_name .literal.esp_netif_get_netif_impl_name_api .literal.esp_netif_get_old_ip_info .literal.esp_netif_init .literal.esp_netif_ip6_get_addr_type .literal.esp_netif_ip_lost_timer .literal.esp_netif_join_ip6_multicast_group .literal.esp_netif_join_ip6_multicast_group_api .literal.esp_netif_leave_ip6_multicast_group .literal.esp_netif_leave_ip6_multicast_group_api .literal.esp_netif_lwip_remove .literal.esp_netif_napt_control_api .literal.esp_netif_napt_disable .literal.esp_netif_napt_enable .literal.esp_netif_netstack_buf_free .literal.esp_netif_netstack_buf_ref .literal.esp_netif_new .literal.esp_netif_new_api .literal.esp_netif_ppp_set_auth .literal.esp_netif_ppp_set_auth_api .literal.esp_netif_remove_ip6_address .literal.esp_netif_remove_ip6_address_api .literal.esp_netif_set_default_netif .literal.esp_netif_set_dns_info .literal.esp_netif_set_dns_info_api .literal.esp_netif_set_driver_config .literal.esp_netif_set_hostname .literal.esp_netif_set_hostname_api .literal.esp_netif_set_ip_info .literal.esp_netif_set_ip_info_api .literal.esp_netif_set_ip_old_info_api .literal.esp_netif_set_mac .literal.esp_netif_set_mac_api .literal.esp_netif_set_old_ip_info .literal.esp_netif_start .literal.esp_netif_start_api .literal.esp_netif_stop .literal.esp_netif_stop_api .literal.esp_netif_str_to_ip4 .literal.esp_netif_str_to_ip6 .literal.esp_netif_tcpip_exec .literal.esp_netif_up .literal.esp_netif_up_api .literal.esp_netif_update_default_netif_lwip .literal.get_handle_from_ifkey_api .literal.netif_callback_fn .literal.netif_send_garp .literal.netif_send_mldv6 .literal.tcpip_init_done .text .text.alloc_netif_client_id .text.esp_ip4addr_aton .text.esp_ip4addr_ntoa .text.esp_netif_add_ip6_address .text.esp_netif_add_ip6_address_api .text.esp_netif_api_cb .text.esp_netif_attach .text.esp_netif_bridge_add_port .text.esp_netif_bridge_fdb_add .text.esp_netif_bridge_fdb_remove .text.esp_netif_create_ip6_linklocal .text.esp_netif_create_ip6_linklocal_api .text.esp_netif_deinit .text.esp_netif_destroy .text.esp_netif_destroy_api .text.esp_netif_dhcpc_get_status .text.esp_netif_dhcpc_option .text.esp_netif_dhcpc_option_api .text.esp_netif_dhcpc_start .text.esp_netif_dhcpc_start_api .text.esp_netif_dhcpc_stop .text.esp_netif_dhcpc_stop_api .text.esp_netif_dhcps_cb .text.esp_netif_dhcps_get_clients_by_mac .text.esp_netif_dhcps_get_clients_by_mac_api .text.esp_netif_dhcps_get_status .text.esp_netif_dhcps_option .text.esp_netif_dhcps_option_api .text.esp_netif_dhcps_start .text.esp_netif_dhcps_start_api .text.esp_netif_dhcps_stop .text.esp_netif_dhcps_stop_api .text.esp_netif_down .text.esp_netif_down_api .text.esp_netif_find_if .text.esp_netif_find_if_api .text.esp_netif_get_all_ip6 .text.esp_netif_get_all_preferred_ip6 .text.esp_netif_get_default_netif .text.esp_netif_get_desc .text.esp_netif_get_dns_info .text.esp_netif_get_dns_info_api .text.esp_netif_get_event_id .text.esp_netif_get_flags .text.esp_netif_get_handle_from_ifkey .text.esp_netif_get_handle_from_netif_impl .text.esp_netif_get_hostname .text.esp_netif_get_ifkey .text.esp_netif_get_io_driver .text.esp_netif_get_ip6_global .text.esp_netif_get_ip6_linklocal .text.esp_netif_get_ip_info .text.esp_netif_get_mac .text.esp_netif_get_netif_impl .text.esp_netif_get_netif_impl_index .text.esp_netif_get_netif_impl_name .text.esp_netif_get_netif_impl_name_api .text.esp_netif_get_old_ip_info .text.esp_netif_get_route_prio .text.esp_netif_init .text.esp_netif_ip6_get_addr_type .text.esp_netif_ip_lost_timer .text.esp_netif_is_netif_up .text.esp_netif_is_valid_static_ip .text.esp_netif_join_ip6_multicast_group .text.esp_netif_join_ip6_multicast_group_api .text.esp_netif_leave_ip6_multicast_group .text.esp_netif_leave_ip6_multicast_group_api .text.esp_netif_lwip_remove .text.esp_netif_napt_control_api .text.esp_netif_napt_disable .text.esp_netif_napt_enable .text.esp_netif_netstack_buf_free .text.esp_netif_netstack_buf_ref .text.esp_netif_new .text.esp_netif_new_api .text.esp_netif_ppp_set_auth .text.esp_netif_ppp_set_auth_api .text.esp_netif_remove_ip6_address .text.esp_netif_remove_ip6_address_api .text.esp_netif_set_default_netif .text.esp_netif_set_dns_info .text.esp_netif_set_dns_info_api .text.esp_netif_set_driver_config .text.esp_netif_set_hostname .text.esp_netif_set_hostname_api .text.esp_netif_set_ip4_addr .text.esp_netif_set_ip_info .text.esp_netif_set_ip_info_api .text.esp_netif_set_ip_old_info_api .text.esp_netif_set_link_speed .text.esp_netif_set_mac .text.esp_netif_set_mac_api .text.esp_netif_set_old_ip_info .text.esp_netif_start .text.esp_netif_start_api .text.esp_netif_stop .text.esp_netif_stop_api .text.esp_netif_str_to_ip4 .text.esp_netif_str_to_ip6 .text.esp_netif_tcpip_exec .text.esp_netif_transmit .text.esp_netif_up .text.esp_netif_up_api .text.esp_netif_update_default_netif_lwip .text.get_handle_from_ifkey_api .text.netif_callback_fn .text.netif_send_garp .text.netif_send_mldv6 .text.tcpip_exec_api .text.tcpip_init_done)
    *libesp_netif.a:esp_pbuf_ref.*(.text)
    *libesp_netif.a:ethernetif.*(.literal.ethernetif_init .text .text.ethernetif_init)
    *libesp_netif.a:wlanif.*(.literal.wlanif_init .literal.wlanif_init_ap .literal.wlanif_init_nan .literal.wlanif_init_sta .text .text.wlanif_init .text.wlanif_init_ap .text.wlanif_init_nan .text.wlanif_init_sta)
    *libesp_system.a:esp_system_chip.*(.literal.esp_get_free_heap_size .literal.esp_get_free_internal_heap_size .literal.esp_get_idf_version .literal.esp_get_minimum_free_heap_size .text .text.esp_get_free_heap_size .text.esp_get_free_internal_heap_size .text.esp_get_idf_version .text.esp_get_minimum_free_heap_size)
    *libfreertos.a:app_startup.*(.literal .literal.* .text .text.*)
    *libfreertos.a:idf_additions.*(.literal .literal.* .text .text.*)
    *libhal.a:timer_hal.*(.literal.timer_hal_deinit .literal.timer_hal_init .literal.timer_hal_set_counter_value .text .text.timer_hal_deinit .text.timer_hal_init .text.timer_hal_set_counter_value)
    *libheap.a:multi_heap.*(.literal.multi_heap_check .literal.multi_heap_dump .literal.multi_heap_dump_tlsf .literal.multi_heap_get_info_impl .literal.multi_heap_register_impl .literal.tlsf_check_hook .text .text.multi_heap_check .text.multi_heap_dump .text.multi_heap_dump_tlsf .text.multi_heap_free_size_impl .text.multi_heap_get_info_impl .text.multi_heap_get_info_tlsf .text.multi_heap_minimum_free_size_impl .text.multi_heap_register_impl .text.tlsf_check_hook)
    *libheap.a:multi_heap_poisoning.*(.literal.multi_heap_free_size .literal.multi_heap_get_info .literal.multi_heap_minimum_free_size .literal.multi_heap_register .text .text.multi_heap_free_size .text.multi_heap_get_info .text.multi_heap_minimum_free_size .text.multi_heap_register)
    *libheap.a:tlsf.*(.literal.default_walker .literal.integrity_walker .literal.tlsf_add_pool .literal.tlsf_check .literal.tlsf_check_pool .literal.tlsf_create .literal.tlsf_create_with_pool .literal.tlsf_fit_size .literal.tlsf_remove_pool .literal.tlsf_walk_pool .text .text.default_walker .text.integrity_walker .text.tlsf_add_pool .text.tlsf_check .text.tlsf_check_pool .text.tlsf_create .text.tlsf_create_with_pool .text.tlsf_destroy .text.tlsf_fit_size .text.tlsf_pool_overhead .text.tlsf_remove_pool .text.tlsf_walk_pool)
    *liblog.a:log.*(.literal.esp_log_level_get .literal.esp_log_level_set .literal.esp_log_set_vprintf .literal.esp_log_writev .literal.heap_bubble_down .literal.s_log_level_get_and_unlock .text .text.esp_log_level_get .text.esp_log_level_set .text.esp_log_set_vprintf .text.esp_log_writev .text.heap_bubble_down .text.s_log_level_get_and_unlock)
    *liblog.a:log_freertos.*(.literal.esp_log_system_timestamp .text .text.esp_log_system_timestamp)
    *liblwip.a:api_lib.*(.literal.netconn_accept .literal.netconn_bind .literal.netconn_bind_if .literal.netconn_close .literal.netconn_connect .literal.netconn_delete .literal.netconn_disconnect .literal.netconn_err .literal.netconn_getaddr .literal.netconn_gethostbyname_addrtype .literal.netconn_join_leave_group .literal.netconn_join_leave_group_netif .literal.netconn_listen_with_backlog .literal.netconn_new_with_proto_and_callback .literal.netconn_prepare_delete .literal.netconn_recv_tcp_pbuf .literal.netconn_recv_udp_raw_netbuf .literal.netconn_shutdown .literal.netconn_thread_cleanup .literal.netconn_thread_init .text .text.netconn_accept .text.netconn_bind .text.netconn_bind_if .text.netconn_close .text.netconn_connect .text.netconn_delete .text.netconn_disconnect .text.netconn_err .text.netconn_getaddr .text.netconn_gethostbyname_addrtype .text.netconn_join_leave_group .text.netconn_join_leave_group_netif .text.netconn_listen_with_backlog .text.netconn_new_with_proto_and_callback .text.netconn_prepare_delete .text.netconn_recv_tcp_pbuf .text.netconn_recv_udp_raw_netbuf .text.netconn_shutdown .text.netconn_thread_cleanup .text.netconn_thread_init)
    *liblwip.a:api_msg.*(.literal.accept_function .literal.err_tcp .literal.lwip_netconn_do_accepted .literal.lwip_netconn_do_bind .literal.lwip_netconn_do_bind_if .literal.lwip_netconn_do_close .literal.lwip_netconn_do_close_internal$isra$0 .literal.lwip_netconn_do_connect .literal.lwip_netconn_do_connected .literal.lwip_netconn_do_delconn .literal.lwip_netconn_do_disconnect .literal.lwip_netconn_do_dns_found .literal.lwip_netconn_do_getaddr .literal.lwip_netconn_do_gethostbyname .literal.lwip_netconn_do_join_leave_group .literal.lwip_netconn_do_join_leave_group_netif .literal.lwip_netconn_do_listen .literal.lwip_netconn_do_newconn .literal.lwip_netconn_do_recv .literal.lwip_netconn_do_writemore$isra$0 .literal.lwip_netconn_is_deallocated_msg .literal.lwip_netconn_is_err_msg .literal.netconn_alloc .literal.netconn_drain .literal.netconn_free .literal.netconn_mark_mbox_invalid .literal.poll_tcp .literal.recv_raw .literal.recv_tcp .literal.sent_tcp .literal.setup_tcp .literal.tcp_do_abort .text .text.accept_function .text.err_tcp .text.lwip_netconn_do_accepted .text.lwip_netconn_do_bind .text.lwip_netconn_do_bind_if .text.lwip_netconn_do_close .text.lwip_netconn_do_close_internal$isra$0 .text.lwip_netconn_do_connect .text.lwip_netconn_do_connected .text.lwip_netconn_do_delconn .text.lwip_netconn_do_disconnect .text.lwip_netconn_do_dns_found .text.lwip_netconn_do_getaddr .text.lwip_netconn_do_gethostbyname .text.lwip_netconn_do_join_leave_group .text.lwip_netconn_do_join_leave_group_netif .text.lwip_netconn_do_listen .text.lwip_netconn_do_newconn .text.lwip_netconn_do_recv .text.lwip_netconn_do_writemore$isra$0 .text.lwip_netconn_is_deallocated_msg .text.lwip_netconn_is_err_msg .text.netconn_alloc .text.netconn_drain .text.netconn_free .text.netconn_mark_mbox_invalid .text.poll_tcp .text.recv_raw .text.recv_tcp .text.sent_tcp .text.setup_tcp .text.tcp_do_abort)
    *liblwip.a:etharp.*(.literal.etharp_cleanup_netif .literal.etharp_find_addr .literal.etharp_find_entry .literal.etharp_free_entry .literal.etharp_get_entry .literal.etharp_input .literal.etharp_query .literal.etharp_raw .literal.etharp_request .literal.etharp_tmr .text .text.etharp_cleanup_netif .text.etharp_find_addr .text.etharp_find_entry .text.etharp_free_entry .text.etharp_get_entry .text.etharp_input .text.etharp_query .text.etharp_raw .text.etharp_request .text.etharp_tmr)
    *liblwip.a:ethernet.*(.text)
    *liblwip.a:inet_chksum.*(.literal.inet_chksum_pseudo_partial .literal.inet_cksum_pseudo_partial_base .literal.ip6_chksum_pseudo .literal.ip6_chksum_pseudo_partial .literal.ip_chksum_pseudo_partial .text .text.inet_chksum_pseudo_partial .text.inet_cksum_pseudo_partial_base .text.ip6_chksum_pseudo .text.ip6_chksum_pseudo_partial .text.ip_chksum_pseudo_partial)
    *liblwip.a:ip4.*(.literal.ip4_set_default_multicast_netif .text .text.ip4_set_default_multicast_netif)
    *liblwip.a:ip4_addr.*(.literal.ip4_addr_netmask_valid .literal.ip4addr_aton .literal.ip4addr_ntoa .literal.ip4addr_ntoa_r .literal.ipaddr_addr .text .text.ip4_addr_netmask_valid .text.ip4addr_aton .text.ip4addr_ntoa .text.ip4addr_ntoa_r .text.ipaddr_addr)
    *liblwip.a:lwip_default_hooks.*(.literal.dhcp_append_extra_opts .literal.dhcp_parse_extra_opts .literal.ip4_netif_exist .literal.lwip_hook_ip6_input .text .text.dhcp_append_extra_opts .text.dhcp_parse_extra_opts .text.ip4_netif_exist .text.lwip_hook_ip6_input)
    *liblwip.a:mem.*(.literal.mem_calloc .literal.mem_free .text .text.mem_calloc .text.mem_free .text.mem_init .text.mem_trim)
    *liblwip.a:memp.*(.literal.do_memp_free_pool$part$0 .literal.do_memp_malloc_pool .literal.memp_free .literal.memp_free_pool .literal.memp_malloc_pool .text .text.do_memp_free_pool$part$0 .text.do_memp_malloc_pool .text.memp_free .text.memp_free_pool .text.memp_init .text.memp_init_pool .text.memp_malloc_pool)
    *liblwip.a:netbuf.*(.literal.netbuf_chain .literal.netbuf_delete .literal.netbuf_new .literal.netbuf_ref .text .text.netbuf_chain .text.netbuf_data .text.netbuf_delete .text.netbuf_first .text.netbuf_new .text.netbuf_next .text.netbuf_ref)
    *liblwip.a:pbuf.*(.literal.pbuf_add_header_force .literal.pbuf_alloc_reference .literal.pbuf_cat .literal.pbuf_chain .literal.pbuf_clone$part$0 .literal.pbuf_coalesce .literal.pbuf_copy_partial .literal.pbuf_dechain .literal.pbuf_free_header .literal.pbuf_free_ooseq_callback .literal.pbuf_get_contiguous .literal.pbuf_header_force .literal.pbuf_memcmp .literal.pbuf_memfind .literal.pbuf_realloc .literal.pbuf_ref .literal.pbuf_ref$part$0 .literal.pbuf_strstr .literal.pbuf_take .literal.pbuf_take_at .text .text.pbuf_add_header_force .text.pbuf_alloc_reference .text.pbuf_cat .text.pbuf_chain .text.pbuf_clen .text.pbuf_clone$part$0 .text.pbuf_coalesce .text.pbuf_copy_partial .text.pbuf_dechain .text.pbuf_free_header .text.pbuf_free_ooseq_callback .text.pbuf_get_at .text.pbuf_get_contiguous .text.pbuf_header_force .text.pbuf_memcmp .text.pbuf_memfind .text.pbuf_put_at .text.pbuf_realloc .text.pbuf_ref .text.pbuf_ref$part$0 .text.pbuf_skip .text.pbuf_strstr .text.pbuf_take .text.pbuf_take_at .text.pbuf_try_get_at)
    *liblwip.a:raw.*(.literal.raw_bind .literal.raw_connect .literal.raw_disconnect .literal.raw_netif_ip_addr_changed .literal.raw_new .literal.raw_new_ip_type .literal.raw_remove .literal.raw_send .literal.raw_sendto .literal.raw_sendto_if_src .text .text.raw_bind .text.raw_bind_netif .text.raw_connect .text.raw_disconnect .text.raw_netif_ip_addr_changed .text.raw_new .text.raw_new_ip_type .text.raw_recv .text.raw_remove .text.raw_send .text.raw_sendto .text.raw_sendto_if_src)
    *liblwip.a:sockets.*(.literal.alloc_socket .literal.free_socket .literal.lwip_accept .literal.lwip_bind .literal.lwip_close .literal.lwip_connect .literal.lwip_fcntl .literal.lwip_getaddrname .literal.lwip_getpeername .literal.lwip_getsockname .literal.lwip_getsockopt .literal.lwip_getsockopt_callback .literal.lwip_inet_ntop .literal.lwip_inet_pton .literal.lwip_ioctl .literal.lwip_listen .literal.lwip_poll .literal.lwip_pollscan .literal.lwip_read .literal.lwip_readv .literal.lwip_recv .literal.lwip_recvfrom_udp_raw$constprop$0 .literal.lwip_recvmsg .literal.lwip_select_inc_sockets_used_set$part$0 .literal.lwip_selscan .literal.lwip_sendmsg .literal.lwip_setsockopt .literal.lwip_setsockopt_callback .literal.lwip_shutdown .literal.lwip_sock_make_addr$isra$0 .literal.lwip_socket .literal.lwip_socket_dbg_get_socket .literal.lwip_socket_thread_cleanup .literal.lwip_socket_thread_init .literal.lwip_unlink_select_cb .literal.lwip_write .literal.lwip_writev .literal.sockaddr_to_ipaddr_port .literal.tryget_socket_unconn .text .text.alloc_socket .text.free_socket .text.lwip_accept .text.lwip_bind .text.lwip_close .text.lwip_connect .text.lwip_fcntl .text.lwip_getaddrname .text.lwip_getpeername .text.lwip_getsockname .text.lwip_getsockopt .text.lwip_getsockopt_callback .text.lwip_inet_ntop .text.lwip_inet_pton .text.lwip_ioctl .text.lwip_listen .text.lwip_poll .text.lwip_pollscan .text.lwip_read .text.lwip_readv .text.lwip_recv .text.lwip_recvfrom_udp_raw$constprop$0 .text.lwip_recvmsg .text.lwip_select_inc_sockets_used_set$part$0 .text.lwip_selscan .text.lwip_sendmsg .text.lwip_setsockopt .text.lwip_setsockopt_callback .text.lwip_shutdown .text.lwip_sock_make_addr$isra$0 .text.lwip_socket .text.lwip_socket_dbg_get_socket .text.lwip_socket_thread_cleanup .text.lwip_socket_thread_init .text.lwip_unlink_select_cb .text.lwip_write .text.lwip_writev .text.sockaddr_to_ipaddr_port .text.tryget_socket_unconn)
    *liblwip.a:sys_arch.*(.literal.sys_arch_mbox_tryfetch .literal.sys_delay_ms .literal.sys_init .literal.sys_jiffies .literal.sys_mbox_free .literal.sys_mbox_new .literal.sys_mbox_trypost_fromisr .literal.sys_mutex_free .literal.sys_mutex_lock$part$0 .literal.sys_mutex_new .literal.sys_mutex_unlock$part$0 .literal.sys_now .literal.sys_sem_free .literal.sys_sem_new .literal.sys_sem_signal_isr .literal.sys_thread_new .literal.sys_thread_sem_deinit .literal.sys_thread_sem_free .literal.sys_thread_sem_get .literal.sys_thread_sem_init .text .text.sys_arch_mbox_tryfetch .text.sys_delay_ms .text.sys_init .text.sys_jiffies .text.sys_mbox_free .text.sys_mbox_new .text.sys_mbox_set_owner .text.sys_mbox_trypost_fromisr .text.sys_mutex_free .text.sys_mutex_lock$part$0 .text.sys_mutex_new .text.sys_mutex_unlock$part$0 .text.sys_now .text.sys_sem_free .text.sys_sem_new .text.sys_sem_signal_isr .text.sys_thread_new .text.sys_thread_sem_deinit .text.sys_thread_sem_free .text.sys_thread_sem_get .text.sys_thread_sem_init)
    *liblwip.a:tcp.*(.literal.tcp_abandon .literal.tcp_abort .literal.tcp_accept_null .literal.tcp_alloc .literal.tcp_backlog_accepted .literal.tcp_backlog_delayed .literal.tcp_bind .literal.tcp_close .literal.tcp_close_ext .literal.tcp_close_shutdown .literal.tcp_close_shutdown_fin .literal.tcp_connect .literal.tcp_debug_state_str .literal.tcp_eff_send_mss_netif .literal.tcp_err .literal.tcp_free .literal.tcp_free$part$0 .literal.tcp_free_ooseq .literal.tcp_init .literal.tcp_kill_state .literal.tcp_listen_with_backlog .literal.tcp_listen_with_backlog_and_err .literal.tcp_netif_ip_addr_changed .literal.tcp_netif_ip_addr_changed_pcblist .literal.tcp_new .literal.tcp_new_ip_type .literal.tcp_new_port .literal.tcp_next_iss .literal.tcp_pcb_purge .literal.tcp_pcb_remove .literal.tcp_poll .literal.tcp_recv .literal.tcp_sent .literal.tcp_shutdown .literal.tcp_tcp_get_tcp_addrinfo .literal.tcp_txnow .text .text.tcp_abandon .text.tcp_abort .text.tcp_accept .text.tcp_accept_null .text.tcp_alloc .text.tcp_arg .text.tcp_backlog_accepted .text.tcp_backlog_delayed .text.tcp_bind .text.tcp_bind_netif .text.tcp_close .text.tcp_close_ext .text.tcp_close_shutdown .text.tcp_close_shutdown_fin .text.tcp_connect .text.tcp_debug_state_str .text.tcp_eff_send_mss_netif .text.tcp_err .text.tcp_free .text.tcp_free$part$0 .text.tcp_free_ooseq .text.tcp_init .text.tcp_kill_state .text.tcp_listen_with_backlog .text.tcp_listen_with_backlog_and_err .text.tcp_netif_ip_addr_changed .text.tcp_netif_ip_addr_changed_pcblist .text.tcp_new .text.tcp_new_ip_type .text.tcp_new_port .text.tcp_next_iss .text.tcp_pcb_purge .text.tcp_pcb_remove .text.tcp_poll .text.tcp_recv .text.tcp_sent .text.tcp_setprio .text.tcp_shutdown .text.tcp_tcp_get_tcp_addrinfo .text.tcp_txnow)
    *liblwip.a:tcp_in.*(.literal.tcp_free_acked_segments$constprop$0 .literal.tcp_trigger_input_pcb_close .text .text.tcp_free_acked_segments$constprop$0 .text.tcp_trigger_input_pcb_close)
    *liblwip.a:tcp_out.*(.literal.tcp_enqueue_flags .literal.tcp_keepalive .literal.tcp_output .literal.tcp_output_alloc_header$constprop$0 .literal.tcp_output_alloc_header_common .literal.tcp_output_segment_busy$part$0 .literal.tcp_rexmit_rto .literal.tcp_rexmit_rto_commit .literal.tcp_rexmit_rto_commit$part$0 .literal.tcp_send_fin .literal.tcp_zero_window_probe .text .text.tcp_enqueue_flags .text.tcp_keepalive .text.tcp_output .text.tcp_output_alloc_header$constprop$0 .text.tcp_output_alloc_header_common .text.tcp_output_segment_busy$part$0 .text.tcp_rexmit_rto .text.tcp_rexmit_rto_commit .text.tcp_rexmit_rto_commit$part$0 .text.tcp_send_fin .text.tcp_zero_window_probe)
    *liblwip.a:tcpip.*(.literal.mem_free_callback .literal.pbuf_free_callback .literal.pbuf_free_int .literal.tcpip_api_call .literal.tcpip_callbackmsg_delete .literal.tcpip_callbackmsg_new .literal.tcpip_callbackmsg_trycallback .literal.tcpip_callbackmsg_trycallback_fromisr .literal.tcpip_init .text .text.mem_free_callback .text.pbuf_free_callback .text.pbuf_free_int .text.tcpip_api_call .text.tcpip_callbackmsg_delete .text.tcpip_callbackmsg_new .text.tcpip_callbackmsg_trycallback .text.tcpip_callbackmsg_trycallback_fromisr .text.tcpip_init)
    *liblwip.a:udp.*(.literal.udp_bind .literal.udp_connect .literal.udp_disconnect .literal.udp_init .literal.udp_netif_ip_addr_changed .literal.udp_new .literal.udp_new_ip_type .literal.udp_remove .text .text.udp_bind .text.udp_bind_netif .text.udp_connect .text.udp_disconnect .text.udp_init .text.udp_netif_ip_addr_changed .text.udp_new .text.udp_new_ip_type .text.udp_recv .text.udp_remove)

    *(.stub .gnu.warning .gnu.linkonce.literal.* .gnu.linkonce.t.*.literal .gnu.linkonce.t.*)
    *(.irom0.text) /* catch stray ICACHE_RODATA_ATTR */
    *(.fini.literal)
    *(.fini)
    *(.gnu.version)

    /** CPU will try to prefetch up to 16 bytes of
      * of instructions. This means that any configuration (e.g. MMU, PMS) must allow
      * safe access to up to 16 bytes after the last real instruction, add
      * dummy bytes to ensure this
      */
    . += _esp_flash_mmap_prefetch_pad_size;

    _text_end = ABSOLUTE(.);
    _instruction_reserved_end = ABSOLUTE(.);  /* This is a symbol marking the flash.text end, this can be used for mmu driver to maintain virtual address */
    _etext = .;

    /**
     * Similar to _iram_start, this symbol goes here so it is
     * resolved by addr2line in preference to the first symbol in
     * the flash.text segment.
     */
    _flash_cache_start = ABSOLUTE(0);
  } > default_code_seg

  /**
   * This dummy section represents the .flash.text section but in default_rodata_seg.
   * Thus, it must have its alignment and (at least) its size.
   */
  .flash_rodata_dummy (NOLOAD):
  {
    _flash_rodata_dummy_start = ABSOLUTE(.);
    /* Start at the same alignment constraint than .flash.text */
    . = ALIGN(ALIGNOF(.flash.text));
    /* Create an empty gap as big as .flash.text section */
    . = . + SIZEOF(.flash.text);
    /* Prepare the alignment of the section above. Few bytes (0x20) must be
     * added for the mapping header. */
    . = ALIGN(_esp_mmu_block_size) + 0x20;
  } > default_rodata_seg

  .flash.appdesc : ALIGN(0x10)
  {
    _rodata_reserved_start = ABSOLUTE(.);  /* This is a symbol marking the flash.rodata start, this can be used for mmu driver to maintain virtual address */
    _rodata_start = ABSOLUTE(.);

    *(.rodata_desc .rodata_desc.*)               /* Should be the first.  App version info.        DO NOT PUT ANYTHING BEFORE IT! */
    *(.rodata_custom_desc .rodata_custom_desc.*) /* Should be the second. Custom app version info. DO NOT PUT ANYTHING BEFORE IT! */

    /* Create an empty gap within this section. Thanks to this, the end of this
     * section will match .flah.rodata's begin address. Thus, both sections
     * will be merged when creating the final bin image. */
    . = ALIGN(ALIGNOF(.flash.rodata));
  } >default_rodata_seg

  .flash.rodata : ALIGN(0x10)
  {
    _flash_rodata_start = ABSOLUTE(.);

    *(EXCLUDE_FILE(*libgcov.a *libphy.a *libapp_trace.a:app_trace.* *libapp_trace.a:app_trace_util.* *libapp_trace.a:port_uart.* *libesp_hw_support.a:esp_memory_utils.* *libesp_hw_support.a:mspi_timing_config.* *libesp_hw_support.a:mspi_timing_tuning.* *libesp_hw_support.a:rtc_clk.* *libesp_hw_support.a:sleep_console.* *libesp_hw_support.a:systimer.* *libesp_mm.a:esp_cache.* *libesp_psram.a:esp_psram_impl_octal.* *libesp_psram.a:mmu_psram_flash.* *libesp_rom.a:esp_rom_cache_esp32s2_esp32s3.* *libesp_rom.a:esp_rom_cache_writeback_esp32s3.* *libesp_rom.a:esp_rom_spiflash.* *libesp_rom.a:esp_rom_systimer.* *libesp_rom.a:esp_rom_wdt.* *libesp_system.a:esp_err.* *libesp_system.a:ubsan.* *libfreertos.a:FreeRTOS-openocd.* *libgcc.a:_divsf3.* *libhal.a:cache_hal.* *libhal.a:i2c_hal_iram.* *libhal.a:ledc_hal_iram.* *libhal.a:mmu_hal.* *libhal.a:spi_flash_encrypt_hal_iram.* *libhal.a:spi_flash_hal_gpspi.* *libhal.a:spi_flash_hal_iram.* *libhal.a:spi_hal_iram.* *libhal.a:spi_slave_hal_iram.* *libhal.a:systimer_hal.* *libnewlib.a:abort.* *libnewlib.a:assert.* *libnewlib.a:heap.* *libnewlib.a:stdatomic.* *libsoc.a:lldesc.* *libspi_flash.a:flash_brownout_hook.* *libspi_flash.a:memspi_host_driver.* *libspi_flash.a:spi_flash_chip_boya.* *libspi_flash.a:spi_flash_chip_gd.* *libspi_flash.a:spi_flash_chip_generic.* *libspi_flash.a:spi_flash_chip_issi.* *libspi_flash.a:spi_flash_chip_mxic.* *libspi_flash.a:spi_flash_chip_mxic_opi.* *libspi_flash.a:spi_flash_chip_th.* *libspi_flash.a:spi_flash_chip_winbond.* *libspi_flash.a:spi_flash_hpm_enable.* *libspi_flash.a:spi_flash_oct_flash_init.* *libspi_flash.a:spi_flash_wrap.*) .rodata EXCLUDE_FILE(*libgcov.a *libphy.a *libapp_trace.a:app_trace.* *libapp_trace.a:app_trace_util.* *libapp_trace.a:port_uart.* *libesp_hw_support.a:esp_memory_utils.* *libesp_hw_support.a:mspi_timing_config.* *libesp_hw_support.a:mspi_timing_tuning.* *libesp_hw_support.a:rtc_clk.* *libesp_hw_support.a:sleep_console.* *libesp_hw_support.a:systimer.* *libesp_mm.a:esp_cache.* *libesp_psram.a:esp_psram_impl_octal.* *libesp_psram.a:mmu_psram_flash.* *libesp_rom.a:esp_rom_cache_esp32s2_esp32s3.* *libesp_rom.a:esp_rom_cache_writeback_esp32s3.* *libesp_rom.a:esp_rom_spiflash.* *libesp_rom.a:esp_rom_systimer.* *libesp_rom.a:esp_rom_wdt.* *libesp_system.a:esp_err.* *libesp_system.a:ubsan.* *libfreertos.a:FreeRTOS-openocd.* *libgcc.a:_divsf3.* *libhal.a:cache_hal.* *libhal.a:i2c_hal_iram.* *libhal.a:ledc_hal_iram.* *libhal.a:mmu_hal.* *libhal.a:spi_flash_encrypt_hal_iram.* *libhal.a:spi_flash_hal_gpspi.* *libhal.a:spi_flash_hal_iram.* *libhal.a:spi_hal_iram.* *libhal.a:spi_slave_hal_iram.* *libhal.a:systimer_hal.* *libnewlib.a:abort.* *libnewlib.a:assert.* *libnewlib.a:heap.* *libnewlib.a:stdatomic.* *libsoc.a:lldesc.* *libspi_flash.a:flash_brownout_hook.* *libspi_flash.a:memspi_host_driver.* *libspi_flash.a:spi_flash_chip_boya.* *libspi_flash.a:spi_flash_chip_gd.* *libspi_flash.a:spi_flash_chip_generic.* *libspi_flash.a:spi_flash_chip_issi.* *libspi_flash.a:spi_flash_chip_mxic.* *libspi_flash.a:spi_flash_chip_mxic_opi.* *libspi_flash.a:spi_flash_chip_th.* *libspi_flash.a:spi_flash_chip_winbond.* *libspi_flash.a:spi_flash_hpm_enable.* *libspi_flash.a:spi_flash_oct_flash_init.* *libspi_flash.a:spi_flash_wrap.*) .rodata.* EXCLUDE_FILE(*libgcov.a *libphy.a *libapp_trace.a:app_trace.* *libapp_trace.a:app_trace_util.* *libapp_trace.a:port_uart.* *libesp_hw_support.a:esp_memory_utils.* *libesp_hw_support.a:mspi_timing_config.* *libesp_hw_support.a:mspi_timing_tuning.* *libesp_hw_support.a:rtc_clk.* *libesp_hw_support.a:sleep_console.* *libesp_hw_support.a:systimer.* *libesp_mm.a:esp_cache.* *libesp_psram.a:esp_psram_impl_octal.* *libesp_psram.a:mmu_psram_flash.* *libesp_rom.a:esp_rom_cache_esp32s2_esp32s3.* *libesp_rom.a:esp_rom_cache_writeback_esp32s3.* *libesp_rom.a:esp_rom_spiflash.* *libesp_rom.a:esp_rom_systimer.* *libesp_rom.a:esp_rom_wdt.* *libesp_system.a:esp_err.* *libesp_system.a:ubsan.* *libfreertos.a:FreeRTOS-openocd.* *libgcc.a:_divsf3.* *libhal.a:cache_hal.* *libhal.a:i2c_hal_iram.* *libhal.a:ledc_hal_iram.* *libhal.a:mmu_hal.* *libhal.a:spi_flash_encrypt_hal_iram.* *libhal.a:spi_flash_hal_gpspi.* *libhal.a:spi_flash_hal_iram.* *libhal.a:spi_hal_iram.* *libhal.a:spi_slave_hal_iram.* *libhal.a:systimer_hal.* *libnewlib.a:abort.* *libnewlib.a:assert.* *libnewlib.a:heap.* *libnewlib.a:stdatomic.* *libsoc.a:lldesc.* *libspi_flash.a:flash_brownout_hook.* *libspi_flash.a:memspi_host_driver.* *libspi_flash.a:spi_flash_chip_boya.* *libspi_flash.a:spi_flash_chip_gd.* *libspi_flash.a:spi_flash_chip_generic.* *libspi_flash.a:spi_flash_chip_issi.* *libspi_flash.a:spi_flash_chip_mxic.* *libspi_flash.a:spi_flash_chip_mxic_opi.* *libspi_flash.a:spi_flash_chip_th.* *libspi_flash.a:spi_flash_chip_winbond.* *libspi_flash.a:spi_flash_hpm_enable.* *libspi_flash.a:spi_flash_oct_flash_init.* *libspi_flash.a:spi_flash_wrap.*) .sdata2 EXCLUDE_FILE(*libgcov.a *libphy.a *libapp_trace.a:app_trace.* *libapp_trace.a:app_trace_util.* *libapp_trace.a:port_uart.* *libesp_hw_support.a:esp_memory_utils.* *libesp_hw_support.a:mspi_timing_config.* *libesp_hw_support.a:mspi_timing_tuning.* *libesp_hw_support.a:rtc_clk.* *libesp_hw_support.a:sleep_console.* *libesp_hw_support.a:systimer.* *libesp_mm.a:esp_cache.* *libesp_psram.a:esp_psram_impl_octal.* *libesp_psram.a:mmu_psram_flash.* *libesp_rom.a:esp_rom_cache_esp32s2_esp32s3.* *libesp_rom.a:esp_rom_cache_writeback_esp32s3.* *libesp_rom.a:esp_rom_spiflash.* *libesp_rom.a:esp_rom_systimer.* *libesp_rom.a:esp_rom_wdt.* *libesp_system.a:esp_err.* *libesp_system.a:ubsan.* *libfreertos.a:FreeRTOS-openocd.* *libgcc.a:_divsf3.* *libhal.a:cache_hal.* *libhal.a:i2c_hal_iram.* *libhal.a:ledc_hal_iram.* *libhal.a:mmu_hal.* *libhal.a:spi_flash_encrypt_hal_iram.* *libhal.a:spi_flash_hal_gpspi.* *libhal.a:spi_flash_hal_iram.* *libhal.a:spi_hal_iram.* *libhal.a:spi_slave_hal_iram.* *libhal.a:systimer_hal.* *libnewlib.a:abort.* *libnewlib.a:assert.* *libnewlib.a:heap.* *libnewlib.a:stdatomic.* *libsoc.a:lldesc.* *libspi_flash.a:flash_brownout_hook.* *libspi_flash.a:memspi_host_driver.* *libspi_flash.a:spi_flash_chip_boya.* *libspi_flash.a:spi_flash_chip_gd.* *libspi_flash.a:spi_flash_chip_generic.* *libspi_flash.a:spi_flash_chip_issi.* *libspi_flash.a:spi_flash_chip_mxic.* *libspi_flash.a:spi_flash_chip_mxic_opi.* *libspi_flash.a:spi_flash_chip_th.* *libspi_flash.a:spi_flash_chip_winbond.* *libspi_flash.a:spi_flash_hpm_enable.* *libspi_flash.a:spi_flash_oct_flash_init.* *libspi_flash.a:spi_flash_wrap.*) .sdata2.* EXCLUDE_FILE(*libgcov.a *libphy.a *libapp_trace.a:app_trace.* *libapp_trace.a:app_trace_util.* *libapp_trace.a:port_uart.* *libesp_hw_support.a:esp_memory_utils.* *libesp_hw_support.a:mspi_timing_config.* *libesp_hw_support.a:mspi_timing_tuning.* *libesp_hw_support.a:rtc_clk.* *libesp_hw_support.a:sleep_console.* *libesp_hw_support.a:systimer.* *libesp_mm.a:esp_cache.* *libesp_psram.a:esp_psram_impl_octal.* *libesp_psram.a:mmu_psram_flash.* *libesp_rom.a:esp_rom_cache_esp32s2_esp32s3.* *libesp_rom.a:esp_rom_cache_writeback_esp32s3.* *libesp_rom.a:esp_rom_spiflash.* *libesp_rom.a:esp_rom_systimer.* *libesp_rom.a:esp_rom_wdt.* *libesp_system.a:esp_err.* *libesp_system.a:ubsan.* *libfreertos.a:FreeRTOS-openocd.* *libgcc.a:_divsf3.* *libhal.a:cache_hal.* *libhal.a:i2c_hal_iram.* *libhal.a:ledc_hal_iram.* *libhal.a:mmu_hal.* *libhal.a:spi_flash_encrypt_hal_iram.* *libhal.a:spi_flash_hal_gpspi.* *libhal.a:spi_flash_hal_iram.* *libhal.a:spi_hal_iram.* *libhal.a:spi_slave_hal_iram.* *libhal.a:systimer_hal.* *libnewlib.a:abort.* *libnewlib.a:assert.* *libnewlib.a:heap.* *libnewlib.a:stdatomic.* *libsoc.a:lldesc.* *libspi_flash.a:flash_brownout_hook.* *libspi_flash.a:memspi_host_driver.* *libspi_flash.a:spi_flash_chip_boya.* *libspi_flash.a:spi_flash_chip_gd.* *libspi_flash.a:spi_flash_chip_generic.* *libspi_flash.a:spi_flash_chip_issi.* *libspi_flash.a:spi_flash_chip_mxic.* *libspi_flash.a:spi_flash_chip_mxic_opi.* *libspi_flash.a:spi_flash_chip_th.* *libspi_flash.a:spi_flash_chip_winbond.* *libspi_flash.a:spi_flash_hpm_enable.* *libspi_flash.a:spi_flash_oct_flash_init.* *libspi_flash.a:spi_flash_wrap.*) .srodata EXCLUDE_FILE(*libgcov.a *libphy.a *libapp_trace.a:app_trace.* *libapp_trace.a:app_trace_util.* *libapp_trace.a:port_uart.* *libesp_hw_support.a:esp_memory_utils.* *libesp_hw_support.a:mspi_timing_config.* *libesp_hw_support.a:mspi_timing_tuning.* *libesp_hw_support.a:rtc_clk.* *libesp_hw_support.a:sleep_console.* *libesp_hw_support.a:systimer.* *libesp_mm.a:esp_cache.* *libesp_psram.a:esp_psram_impl_octal.* *libesp_psram.a:mmu_psram_flash.* *libesp_rom.a:esp_rom_cache_esp32s2_esp32s3.* *libesp_rom.a:esp_rom_cache_writeback_esp32s3.* *libesp_rom.a:esp_rom_spiflash.* *libesp_rom.a:esp_rom_systimer.* *libesp_rom.a:esp_rom_wdt.* *libesp_system.a:esp_err.* *libesp_system.a:ubsan.* *libfreertos.a:FreeRTOS-openocd.* *libgcc.a:_divsf3.* *libhal.a:cache_hal.* *libhal.a:i2c_hal_iram.* *libhal.a:ledc_hal_iram.* *libhal.a:mmu_hal.* *libhal.a:spi_flash_encrypt_hal_iram.* *libhal.a:spi_flash_hal_gpspi.* *libhal.a:spi_flash_hal_iram.* *libhal.a:spi_hal_iram.* *libhal.a:spi_slave_hal_iram.* *libhal.a:systimer_hal.* *libnewlib.a:abort.* *libnewlib.a:assert.* *libnewlib.a:heap.* *libnewlib.a:stdatomic.* *libsoc.a:lldesc.* *libspi_flash.a:flash_brownout_hook.* *libspi_flash.a:memspi_host_driver.* *libspi_flash.a:spi_flash_chip_boya.* *libspi_flash.a:spi_flash_chip_gd.* *libspi_flash.a:spi_flash_chip_generic.* *libspi_flash.a:spi_flash_chip_issi.* *libspi_flash.a:spi_flash_chip_mxic.* *libspi_flash.a:spi_flash_chip_mxic_opi.* *libspi_flash.a:spi_flash_chip_th.* *libspi_flash.a:spi_flash_chip_winbond.* *libspi_flash.a:spi_flash_hpm_enable.* *libspi_flash.a:spi_flash_oct_flash_init.* *libspi_flash.a:spi_flash_wrap.*) .srodata.*)
    *(.rodata_wlog_error .rodata_wlog_error.*)

    *(.irom1.text) /* catch stray ICACHE_RODATA_ATTR */
    *(.gnu.linkonce.r.*)
    *(.rodata1)
    __XT_EXCEPTION_TABLE_ = ABSOLUTE(.);
    *(.xt_except_table)
    *(.gcc_except_table .gcc_except_table.*)
    *(.gnu.linkonce.e.*)
    *(.gnu.version_r)
    . = (. + 3) & ~ 3;
    __eh_frame = ABSOLUTE(.);
    KEEP(*(.eh_frame))
    . = (. + 7) & ~ 3;
    /* C++ constructor and destructor tables */
    /* Don't include anything from crtbegin.o or crtend.o, as IDF doesn't use toolchain crt */
    __init_array_start = ABSOLUTE(.);
    KEEP (*(EXCLUDE_FILE (*crtend.* *crtbegin.*) .ctors SORT(.ctors.*)))
    __init_array_end = ABSOLUTE(.);
    KEEP (*crtbegin.*(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.*) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
    /* C++ exception handlers table: */
    __XT_EXCEPTION_DESCS_ = ABSOLUTE(.);
    *(.xt_except_desc)
    *(.gnu.linkonce.h.*)
    __XT_EXCEPTION_DESCS_END__ = ABSOLUTE(.);
    *(.xt_except_desc_end)
    *(.dynamic)
    *(.gnu.version_d)
    /* Addresses of memory regions reserved via SOC_RESERVE_MEMORY_REGION() */
    soc_reserved_memory_region_start = ABSOLUTE(.);
    KEEP (*(.reserved_memory_address))
    soc_reserved_memory_region_end = ABSOLUTE(.);
    /* System init functions registered via ESP_SYSTEM_INIT_FN */
    _esp_system_init_fn_array_start = ABSOLUTE(.);
    KEEP (*(SORT_BY_INIT_PRIORITY(.esp_system_init_fn.*)))
    _esp_system_init_fn_array_end = ABSOLUTE(.);
    _rodata_end = ABSOLUTE(.);
    /* Literals are also RO data. */
    _lit4_start = ABSOLUTE(.);
    *(*.lit4)
    *(.lit4.*)
    *(.gnu.linkonce.lit4.*)
    _lit4_end = ABSOLUTE(.);
    . = ALIGN(4);
    _thread_local_start = ABSOLUTE(.);
    *(.tdata)
    *(.tdata.*)
    *(.tbss)
    *(.tbss.*)
    _thread_local_end = ABSOLUTE(.);
    . = ALIGN(4);
  } > default_rodata_seg

  _flash_rodata_align = ALIGNOF(.flash.rodata);

  /*
    This section is a place where we dump all the rodata which aren't used at runtime,
    so as to avoid binary size increase
  */
  .flash.rodata_noload (NOLOAD) :
  {
    /*
      This is a symbol marking the flash.rodata end, this can be used for mmu driver to maintain virtual address
      We don't need to include the noload rodata in this section
    */
    _rodata_reserved_end = ABSOLUTE(.);
    . = ALIGN (4);
    *(.rodata_wlog_debug .rodata_wlog_debug.*)
    *(.rodata_wlog_info .rodata_wlog_info.*)
    *(.rodata_wlog_verbose .rodata_wlog_verbose.*)
    *(.rodata_wlog_warning .rodata_wlog_warning.*)
  } > default_rodata_seg

  /**
   * This section is required to skip flash rodata sections, because `extern_ram_seg`
   * and `drom0_0_seg` are on the same bus
   */
  .ext_ram.dummy (NOLOAD):
  {
    . = ORIGIN(extern_ram_seg) + (_rodata_reserved_end - _flash_rodata_dummy_start);
    . = ALIGN (0x10000);
  } > extern_ram_seg

  /* This section holds .ext_ram.bss data, and will be put in PSRAM */
  .ext_ram.bss (NOLOAD) :
  {
    _ext_ram_bss_start = ABSOLUTE(.);
    . = ALIGN(4);
    _ext_ram_bss_end = ABSOLUTE(.);
  } > extern_ram_seg

  /**
   * This section holds data that won't be initialised when startup.
   * This section locates in External RAM region.
   */
  .ext_ram_noinit (NOLOAD) :
  {
    _ext_ram_noinit_start = ABSOLUTE(.);

    *(.ext_ram_noinit*)

    . = ALIGN(4);
    _ext_ram_noinit_end = ABSOLUTE(.);
  } > extern_ram_seg

  /* Marks the end of IRAM code segment */
  .iram0.text_end (NOLOAD) :
  {
    /* iram_end_test section exists for use by memprot unit tests only */
    *(.iram_end_test)
    /* ESP32-S3 memprot requires 16B padding for possible CPU prefetch and 256B alignment for PMS split lines */
    . += _esp_memprot_prefetch_pad_size;
    . = ALIGN(_esp_memprot_align_size);
    _iram_text_end = ABSOLUTE(.);
  } > iram0_0_seg

  .iram0.data :
  {
    . = ALIGN(4);
    _iram_data_start = ABSOLUTE(.);

    *(.iram.data .iram.data.*)
    _coredump_iram_start = ABSOLUTE(.);
    *(.iram2.coredump .iram2.coredump.*)
    _coredump_iram_end = ABSOLUTE(.);

    _iram_data_end = ABSOLUTE(.);
  } > iram0_0_seg

  .iram0.bss (NOLOAD) :
  {
    . = ALIGN(4);
    _iram_bss_start = ABSOLUTE(.);

    *(.iram.bss .iram.bss.*)

    _iram_bss_end = ABSOLUTE(.);
    . = ALIGN(4);
    _iram_end = ABSOLUTE(.);
   } > iram0_0_seg

  /* Marks the end of data, bss and possibly rodata  */
  .dram0.heap_start (NOLOAD) :
  {
    . = ALIGN (8);
    /* Lowest possible start address for the heap */
    _heap_low_start = ABSOLUTE(.);
  } > dram0_0_seg

  /** This section will be used by the debugger and disassembler to get more information
    * about raw data present in the code.
    * Indeed, it may be required to add some padding at some points in the code
    * in order to align a branch/jump destination on a particular bound.
    * Padding these instructions will generate null bytes that shall be
    * interpreted as data, and not code by the debugger or disassembler.
    * This section will only be present in the ELF file, not in the final binary
    * For more details, check GCC-212
    */
  .xt.prop 0 :
  {
    KEEP (*(.xt.prop .gnu.linkonce.prop.*))
  }

  .xt.lit 0 :
  {
    KEEP (*(.xt.lit .gnu.linkonce.p.*))
  }
}

ASSERT(((_iram_end - ORIGIN(iram0_0_seg)) <= LENGTH(iram0_0_seg)),
          "IRAM0 segment data does not fit.")

ASSERT(((_heap_low_start - ORIGIN(dram0_0_seg)) <= LENGTH(dram0_0_seg)),
          "DRAM segment data does not fit.")

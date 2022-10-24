include_directories(
  ${ENV_WCH_BASE}/kernel/include
  ${ENV_WCH_BASE}/arch/${ARCH}/include
  )

add_subdirectory(${ENV_WCH_BASE}/arch/${ARCH})
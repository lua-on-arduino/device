Import("env")

def after_upload(source, target, env):
  if "UNIT_TEST" in env["CPPDEFINES"]:
    import time
    # Give the teensy a little time!
    time.sleep(1)

env.AddPostAction("upload", after_upload)
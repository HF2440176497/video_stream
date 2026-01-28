
import video_stream

def process_frame(meta: video_stream.vp_frame_meta):
    meta.frame_index += 100  # C++成员被修改
    meta.fps = 30
    return None  # 或 return meta


if __name__ == "__main__":
    meta = video_stream.vp_frame_meta()
    process_frame(meta)
    print(meta.frame_index)
    print(meta.fps)

"""
测试：单独运行此 Python 脚本，发现 meta 被同步修改

"""
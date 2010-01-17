package dto.security.video
{
	import mx.collections.ArrayCollection;
	[Bindable]
	[RemoteClass(alias="com.broadengate.vcms.entity.VideoType")]
	public class VideoType
	{
		public var typeId:int;
		public var videoType:VideoType;
		public var typeName:String;
		public var createTime:Date;
		public var videoNum:int;
		public var videoInfos:ArrayCollection;
		public var videoTypes:ArrayCollection;
		public function VideoType()
		{
			//TODO: implement function
		}

	}
}